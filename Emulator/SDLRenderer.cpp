//
//  SDLRenderer.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#include "SDLRenderer.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void drawPixel(SDL_Surface *surface, int x, int y, color_t color) {
    Uint32 *p = (Uint32 *)surface->pixels + y * surface->pitch / 4 + x;
    *p = SDL_MapRGB(surface->format, color.r, color.g, color.b);
}

Uint32 getPixel(SDL_Surface *surface, int x, int y) {
    Uint32 *p = (Uint32 *)surface->pixels + y * surface->pitch / 4 + x;
    return *p;
}

uint8_t SDLRenderer::color_index_for_pattern_bit(int pattern_num, int attr_high_bits, int x, int y, bool sprite) {
    int patternStart = pattern_num * cPATTERN_SIZE;
    
    uint8_t lowerByte = ppu->read_memory(patternStart + y);
    uint8_t higherByte = ppu->read_memory(patternStart + y + 8);
    
    int patternBit = 7 - x; // x is ascending left to right; that's H -> L in bit order
    
    uint8_t palette_entry = (attr_high_bits << 2) |
    ((lowerByte & (1 << patternBit)) >> patternBit) |
    (patternBit == 0 ? ((higherByte & (1 << patternBit)) << 1) :
     ((higherByte & (1 << patternBit)) >> (patternBit - 1)));
    
    return ppu->read_memory(PALETTE_TABLE_START + (sprite ? PALETTE_TABLE_SPRITE_OFFSET : 0) + palette_entry);
}

void SDLRenderer::draw_scanline(int scanline) {
    uint8_t control_1 = ppu->read_control_1();
    uint8_t control_2 = ppu->read_control_2();
    
    ppu->reset_more_than_8_sprites_flag();
    
    int tile_row = scanline / 8;

    ///////////////////////////
    // RENDER THE BACKGROUND //
    ///////////////////////////
    if ((control_2 & BACKGROUND_ENABLE_MASK) == BACKGROUND_ENABLE) {
        for (int tile_column = 0; tile_column < SCREEN_WIDTH / 8; tile_column++) {
            
            int pattern_num = ppu->read_memory(0x2000 + tile_row * 32 + tile_column);
            if ((control_1 & BACKGROUND_PATTERN_TABLE_ADDRESS_MASK) == BACKGROUND_PATTERN_TABLE_ADDRESS_1000) {
                pattern_num += 256;
            }
            
            // Calculate the 2 high bits of the palette offset using the attribute table.
            int attr_byte_address = (tile_row / 4) * 8 + (tile_column / 4);
            int attr_byte = ppu->read_memory(0x23C0 + attr_byte_address);
            int bits_offset = ((tile_row & 0x02) << 1) | (tile_column & 0x02);
            attr_byte = (attr_byte & (0x03 << (bits_offset))) >> bits_offset;
            
            for (int x = 0; x < 8; x++) {
                drawPixel(screen, tile_column * 8 + x, scanline, NES_PALETTE[color_index_for_pattern_bit(pattern_num, attr_byte, x, scanline % 8, false)]);
            }
        }
    }
    
    ////////////////////////
    // RENDER THE SPRITES //
    ////////////////////////
    if ((control_2 & SPRITES_ENABLE_MASK) == SPRITES_ENABLE) {
        if ((control_2 & SPRITE_SIZE_MASK) == SPRITE_SIZE_8x16) {
            throw "Unimplemented sprite size 8x16!";
        }
        
        uint8_t transparency_color_index = ppu->read_memory(PALETTE_TABLE_START);
        color_t transparency_color = NES_PALETTE[transparency_color_index];
        Uint32 transparency_color_sdl = SDL_MapRGB(screen->format, transparency_color.r, transparency_color.b, transparency_color.g);

        int sprites_drawn = 0;
        
        // Lowest number sprites are highest priority to draw
        for (int i = 63; i >= 0; i--) {
            int ypos            = ppu->spr_ram[i * 4] + 1;
            
            if (!(ypos <= scanline && ypos + 7 >= scanline)) {
                // Does this sprite intersect with this scanline?
                continue;
            }
            
            sprites_drawn += 1;
            
            if (sprites_drawn == 9) {
                ppu->set_more_than_8_sprites_flag();
                break;
            }
            
            int pattern_num     = ppu->spr_ram[i * 4 + 1];
            uint8_t color_attr  = ppu->spr_ram[i * 4 + 2];
            uint8_t xpos        = ppu->spr_ram[i * 4 + 3];
            
            if ((control_1 & SPRITE_PATTERN_TABLE_ADDRESS_MASK) == SPRITE_PATTERN_TABLE_ADDRESS_1000) {
                pattern_num += 256;
            }
            
            int upper_color_bits = color_attr & 0x03;
            bool flip_horizontal = color_attr & 0x40;
            bool flip_vertical = color_attr & 0x80;
            
            int y = scanline - ypos;
            for (int x = 0; x < 8; x++) {
                uint8_t color_index =
                    color_index_for_pattern_bit(pattern_num, upper_color_bits, (flip_horizontal ? 7 - x : x), (flip_vertical ? 7 - y : y), true);
                
                if (color_index != transparency_color_index) {
                    Uint32 current_pixel = getPixel(screen, xpos + x, scanline);
                    
                    // Sprite 0 hit flag - TODO: Is this correct??
                    if (i == 0  && current_pixel != transparency_color_sdl) {
                        ppu->set_sprite_0_flag();
                    }
                    
                    // if color_attr & 0x20 == 0x20, sprite is drawn behind background (but not transparent color)
                    if ((color_attr & 0x20) == 0x00 || current_pixel == transparency_color_sdl) {
                        drawPixel(screen, xpos + x, scanline, NES_PALETTE[color_index]);
                    }
                }
            }
        }
    }
}

void SDLRenderer::render() {
    if (SDL_MUSTLOCK(screen)) {
        SDL_LockSurface(screen);
    }

    for (int row = 0; row < SCREEN_HEIGHT; row++) {
        draw_scanline(row);
    }
    
    SDL_Flip(screen);
    
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
}

void SDLRenderer::resize(int width, int height) {
//	glViewport(0, 0, width, height);
//	
//	m_viewWidth = width;
//	m_viewHeight = height;
}

SDLRenderer::SDLRenderer(PPU *ppu) {
    this->ppu = ppu;
    
    // initialize SDL
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0)
    {
        std::cout << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    // create the screen surface
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );// | SDL_FULLSCREEN );
    
    if (!screen)
    {
        std::cout << "Unable to set 256x240 video: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    const std::string window_title = "Emulator";
    
    // set the title bar text
    SDL_WM_SetCaption( window_title.c_str(), window_title.c_str() );
}

SDLRenderer::~SDLRenderer() {
    SDL_Quit();
}

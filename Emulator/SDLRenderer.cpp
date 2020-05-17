//
//  SDLRenderer.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#include "SDLRenderer.h"
#include "PPU.h"

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

static const int kPatternSizeBytes = 16;

void drawPixel(SDL_Surface *surface, int x, int y, color_t color) {
    Uint32 *p = (Uint32 *)surface->pixels + y * surface->pitch / 4 + x;
    *p = SDL_MapRGB(surface->format, color.r, color.g, color.b);
}

Uint32 getPixel(SDL_Surface *surface, int x, int y) {
    Uint32 *p = (Uint32 *)surface->pixels + y * surface->pitch / 4 + x;
    return *p;
}

uint8_t SDLRenderer::color_index_for_pattern_bit(int x, uint16_t pattern_start, int palette_select, bool sprite) {
    // First, calculate the 4-bit palette address
    // The upper two bits come from palette_select (from the attribute table)
    // The lower two bits come from the pattern table (the CHR ROM)
    // x is the horizontal index of the pixel we're looking for
    uint16_t palette_address = kPaletteTableStart;

    uint8_t lower_byte = ppu->read_memory(pattern_start);
    uint8_t higher_byte = ppu->read_memory(pattern_start + 8);

    int pattern_bit = 7 - x; // x is ascending left to right; that's H -> L in bit order

    uint8_t palette_entry =
        ((lower_byte & (1 << pattern_bit)) >> pattern_bit) |
        (pattern_bit == 0 ?
         ((higher_byte & (1 << pattern_bit)) << 1) :
         ((higher_byte & (1 << pattern_bit)) >> (pattern_bit - 1)));

    // If the last two bits are 00 we always read the background color at PALETTE_TABLE_START
    if (palette_entry != 0) {
        palette_entry |= (palette_select << 2);
        palette_address += (sprite ? kPaletteTableSpriteOffset : 0) + palette_entry;
    }

    // Now, use the address to determine the actual color index
    return ppu->read_memory(palette_address);
}

void SDLRenderer::print_pattern(int pattern_num) {
    int pattern_start = pattern_num * kPatternSizeBytes;

    for (int y = 0; y < 8; y++) {
        std::cout
            << std::hex
            << (int) ppu->read_memory(pattern_start + y) + ppu->read_memory(pattern_start + y + 8)
            << std::endl;
    }
}

//
// Render one scanline to the framebuffer.
//
// @param scanline An integer between 0 and 239, inclusive
//
void SDLRenderer::render_scanline(int scanline) {
    if (SDL_MUSTLOCK(screen)) {
        SDL_LockSurface(screen);
    }

    ppu->reset_more_than_8_sprites_flag();

    // TODO: Render Sprites & Background at the same time

    ///////////////////////////
    // RENDER THE BACKGROUND //
    ///////////////////////////
    if (ppu->enable_background()) {
        int x = 0;

        while (x < kScreenWidth) {
            drawPixel(
                screen,
                x,
                scanline,
                NES_PALETTE[
                    color_index_for_pattern_bit(
                        (x + ppu->regFH) % 8, // bit offset within that tile
                        ppu->patterntable_address(),
                        ppu->palette_select_bits(),
                        false
                    )
                ]
            );

            // roll over to the next tile?
            if ((++x + ppu->regFH) % 8 == 0) {
                ppu->increment_horizontal_scroll_counter();
            }
        }
    }

    ////////////////////////
    // RENDER THE SPRITES //
    ////////////////////////
    if (ppu->enable_sprites()) {
        if (ppu->use_8x16_sprites()) {
            throw "Unimplemented sprite size 8x16!";
        }

        uint8_t transparency_color_index = ppu->read_memory(kPaletteTableStart);
        color_t transparency_color = NES_PALETTE[transparency_color_index];
        Uint32 transparency_color_sdl = SDL_MapRGB(screen->format, transparency_color.r, transparency_color.b, transparency_color.g);

        int sprites_drawn = 0;

        // Lowest number sprites are highest priority to draw
        for (int i = 63; i >= 0; i--) {
            int ypos = ppu->spr_ram[i * 4] + 1;

            if (!(ypos <= scanline && ypos + 7 >= scanline)) {
                // Does this sprite intersect with this scanline?
                continue;
            }

            sprites_drawn += 1;

            if (sprites_drawn == 9) {
                ppu->set_more_than_8_sprites_flag();
                break;
            }

            int pattern_base = ppu->sprite_pattern_table_address();

            int pattern_num     = ppu->spr_ram[i * 4 + 1];
            uint8_t color_attr  = ppu->spr_ram[i * 4 + 2];
            uint8_t xpos        = ppu->spr_ram[i * 4 + 3];

            int upper_color_bits = color_attr & 0x03;
            bool flip_horizontal = color_attr & 0x40;
            bool flip_vertical = color_attr & 0x80;

            int y = scanline - ypos;
            for (int x = 0; x < 8; x++) {
                uint16_t pattern_start = pattern_base + pattern_num * kPatternSizeBytes + (flip_vertical ? 7 - y : y);
                uint8_t color_index =
                    color_index_for_pattern_bit((flip_horizontal ? 7 - x : x), pattern_start, upper_color_bits, true);

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

    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }

    if (scanline == 239) {
        SDL_UpdateWindowSurface(window);
    }

    ppu->increment_vertical_scroll_counter();
}

SDLRenderer::SDLRenderer(PPU *ppu) {
    this->ppu = ppu;

    // create the screen surface
    window = SDL_CreateWindow("Emulator",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              kScreenWidth,
                              kScreenHeight,
                              0);

    if (!window) {
        std::cout << "Unable to set 256x240 video: " << SDL_GetError() << std::endl;
        exit(1);
    }

    screen = SDL_GetWindowSurface(window);
}

SDLRenderer::~SDLRenderer() {
    SDL_DestroyWindow(window);
}

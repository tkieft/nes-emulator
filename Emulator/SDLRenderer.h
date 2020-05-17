//
//  OpenGLRenderer.h
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#ifndef SDL_Renderer_h
#define SDL_Renderer_h

#include "SDL.h"

#include "nes_palette.h"

class PPU;

// The pattern tables from 0x0000 - 0x2000 hold 512 8x8 patterns.
// static const int cPATTERNS = 512;

// 16 bytes per pattern
static const int cPATTERN_SIZE = 16;

class SDLRenderer {
private:
    SDL_Window *window;
    SDL_Surface *screen;
    PPU *ppu;
    
    uint8_t color_index_for_pattern_bit(int x, uint16_t pattern_start, int attr_high_bits, bool sprite);
    void print_pattern(int pattern_num);
    
public:
    SDLRenderer(PPU *ppu);
    ~SDLRenderer();
    void render_scanline(int scanline);
};

#endif

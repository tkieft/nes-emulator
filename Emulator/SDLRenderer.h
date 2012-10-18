//
//  OpenGLRenderer.h
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#ifndef SDL_Renderer_h
#define SDL_Renderer_h

#include "SDL/SDL.h"

#include "Renderer.h"
#include "PPU.h"
#include "nes_palette.h"

// The pattern tables from 0x0000 - 0x2000 hold 512 8x8 patterns.
static const int cPATTERNS = 512;

// 16 bytes per pattern
static const int cPATTERN_SIZE = 16;

class SDLRenderer : public Renderer {
private:
    SDL_Surface *screen;
    PPU *ppu;
    
    uint8_t color_index_for_pattern_bit(int pattern_num, int attr_high_bits, int x, int y, bool sprite);

public:
    SDLRenderer(PPU *ppu);
    virtual ~SDLRenderer();
    virtual void resize(int width, int height);
    virtual void render();
};

#endif
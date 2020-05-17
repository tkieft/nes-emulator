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

#include "defines.h"

class PPU;

class SDLRenderer {
private:
    SDL_Window *window;
    SDL_Surface *screen;
    PPU *ppu;

    byte color_index_for_pattern_bit(int x, dbyte pattern_start, int attr_high_bits, bool sprite);

public:
    SDLRenderer(PPU *ppu);
    ~SDLRenderer();
    void render_scanline(int scanline);
};

#endif

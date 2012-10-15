//
//  OpenGLRenderer.h
//  Emulator
//
//  Created by Tyler Kieft on 9/30/12.
//
//

#ifndef Open_GL_Renderer_h
#define Open_GL_Renderer_h

#include "Renderer.h"
#include "PPU.h"
#include "nes_palette.h"

// The pattern tables from 0x0000 - 0x2000 hold 512 8x8 patterns.
static const int cPATTERNS = 512;

// 16 bytes per pattern
static const int cPATTERN_SIZE = 16;

class OpenGLRenderer : public Renderer {
private:
    PPU *ppu;

public:
    OpenGLRenderer(PPU *ppu);
    virtual ~OpenGLRenderer();
    virtual void resize(int width, int height);
    virtual void render();
    virtual void update_patterns();
};

#endif
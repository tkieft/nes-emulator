//
//  TerminalRenderer.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef __Emulator__TerminalRenderer__
#define __Emulator__TerminalRenderer__

#include "Renderer.h"
#include "PPU.h"

class TerminalRenderer : public Renderer {
private:
    PPU *ppu;
public:
    TerminalRenderer(PPU *ppu);
    virtual ~TerminalRenderer();
    virtual void render();
    virtual void resize(int width, int height) { }
};

#endif /* defined(__Emulator__TerminalRenderer__) */

//
//  Emulator.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef Emulator_Emulator_h
#define Emulator_Emulator_h

#include <string>
#include <SDL/SDL_keysym.h>

#include "PPU.h"
#include "Processor.h"
#include "ControllerPad.h"

class Emulator {
private:
    PPU *ppu;
    Processor *processor;
    ControllerPad *controller_pad;
    
public:
    Emulator();
    ~Emulator();
    void load_rom(std::string filename);
    void emulate_frame();
    void resize(int width, int height);
    
    bool handle_key_up(SDLKey sym);
    bool handle_key_down(SDLKey sym);
};

#endif

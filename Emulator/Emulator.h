//
//  Emulator.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef Emulator_Emulator_h
#define Emulator_Emulator_h

#include <memory>
#include <string>

#include "SDL.h"

#include "PPU.h"
#include "Processor.h"
#include "ControllerPad.h"

class Emulator {
private:
    PPU ppu;
    ControllerPad controller_pad;
    std::unique_ptr<Processor> processor;

public:
    Emulator();
    void load_rom(std::string filename);
    void emulate_frame();

    bool handle_key_up(SDL_Keysym sym);
    bool handle_key_down(SDL_Keysym sym);
};

#endif

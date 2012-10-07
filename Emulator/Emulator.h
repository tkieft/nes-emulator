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

#include "PPU.h"
#include "Processor.h"

class Emulator {
private:
    PPU *ppu;
    Processor *processor;
    
public:
    Emulator();
    ~Emulator();
    void load_rom(std::string filename);
    void emulate_frame();
    void resize(GLuint width, GLuint height);
};

#endif

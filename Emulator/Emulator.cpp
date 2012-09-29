/*
 *  Emulator.cpp
 *  Emulator
 *
 *  Created by Tyler Kieft on 9/24/12.
 *
 *
 */

#include <iostream>

#include "PPU.h"
#include "Processor.h"
#include "RomReader.h"

using namespace std;

int main(int argc, char* argv[]) {

    RomReader reader("/Users/tylerk/Downloads/mario.nes");
    PPU ppu;
    Processor processor(&ppu, reader.get_prg_rom());

    while (true) {
        processor.execute();
    }
    
    return 0;
}
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
    int clock = 0;

    RomReader reader("/Users/tylerk/Downloads/mario.nes");
    PPU ppu;
    ppu.set_chr_rom((uint8_t *)reader.get_chr_rom());
    Processor processor(&ppu, reader.get_prg_rom());

    while (true) {
        processor.execute();
        
        clock += 5;
        
        if (clock > 10000) {
            if( ppu.render_screen() )
                processor.non_maskable_interrupt();
            
            clock = 0;
        }
    }
    
    return 0;
}
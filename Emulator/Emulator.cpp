/*
 *  Emulator.cpp
 *  Emulator
 *
 *  Created by Tyler Kieft on 9/24/12.
 *
 *
 */

#include <iostream>

#include "Processor.h"
#include "RomReader.h"

using namespace std;

int main(int argc, char* argv[]) {

    RomReader reader("/Users/tylerk/Downloads/mario.nes");
    
    Processor processor(reader.getPrgRom(), reader.getChrRom());

    
    while (true) {
        processor.execute();
    }
    
    return 0;
}
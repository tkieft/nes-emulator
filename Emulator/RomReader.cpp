//
//  RomReader.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/25/12.
//
//

#include "RomReader.h"

RomReader::RomReader(string filename) {
    
    ifstream file (filename, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        fileSize = file.tellg();
        file.seekg (0, ios::beg);
        file.read(header, HEADER_SIZE);
        
        if (!(header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == '\x1A')) {
            file.close();
            throw "This is not a valid NES ROM!";
        }
        
        prgRomBytes = header[PRG_ROM_BYTE] * PRG_ROM_PAGE_SIZE;
        chrRomBytes = header[CHR_ROM_BYTE] * CHR_ROM_PAGE_SIZE;
        prgRamBytes = header[PRG_RAM_BYTE] * PRG_RAM_PAGE_SIZE;
        if (!prgRamBytes) prgRamBytes = PRG_RAM_PAGE_SIZE;
        
        // Error checking
        if (!prgRomBytes) {
            throw "Program ROM bytes = 0.";
        }
        
        if (HEADER_SIZE + prgRomBytes + chrRomBytes != fileSize) {
            throw "Inconsistent file size.";
        }
        
        prgRom = new char[prgRomBytes];
        file.read(prgRom, prgRomBytes);

        if (chrRomBytes) {
            chrRom = new char[chrRomBytes];
            file.read(chrRom, chrRomBytes);
        }
        
        prgRam = new char[prgRamBytes];
        file.read(prgRam, prgRamBytes);
        
        file.close();
    }
    else {
        throw "Could not open file for reading.";
    }
}

char *RomReader::getPrgRom() {
    return prgRom;
}

char *RomReader::getChrRom() {
    return chrRom;
}

void RomReader::printDebugInfo() {
    std::bitset<8> flags6(header[6]);
    std::bitset<8> flags7(header[7]);
    std::bitset<8> flags9(header[9]);
    std::bitset<8> flags10(header[10]);
    
    cout << "File size is " << fileSize << " bytes." << endl << endl;
    
    cout << header[0] << header[1] << header[2] << endl;
    cout << "Size of PRG ROM: " << prgRomBytes << " bytes" << endl;
    cout << "Size of CHR ROM: " << chrRomBytes << " bytes" << endl;
    cout << "Size of PRG RAM: " << prgRamBytes << " bytes" << endl << endl;
    
    cout << "Flags 6: " << flags6 << endl;
    cout << "Flags 7: " << flags7 << endl;
    cout << "Flags 9: " << flags9 << endl;
    cout << "Flags 10: " << flags10 << endl;
    
    // Mirroring type
    cout << endl << "Mirroring Type: ";
    if (header[6] & FOUR_SCREEN_MIRRORING) {
        cout << "Four-screen" << endl;
    } else if (header[6] & VERTICAL_MIRRORING) {
        cout << "Vertical" << endl;
    } else {
        cout << "Horizontal" << endl;
    }
    
    if (header[6] & BATTERY_BACKED_SRAM ) {
        cout << "Battery backed SRAM in CPU $6000-$7FFF" << endl;
    }
    
    if (header[6] & TRAINER) {
        cout << "512 byte trainer at $7000-$71FF" << endl;
    }
    
    int mapper = (header[6] & MAPPER_MASK) >> 4 + header[7] & MAPPER_MASK;
    if (mapper) {
        cout << "Memory Mapper: " << mapper << endl;
    }
    
    if (header[7] || header[9] || header[10]) {
        cout << "Unimplemented features in flags 7-10!";
    }
}
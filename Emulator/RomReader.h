//
//  RomReader.h
//  Emulator
//
//  Created by Tyler Kieft on 9/25/12.
//
//

#ifndef __Emulator__RomReader__
#define __Emulator__RomReader__

#define HEADER_SIZE 16

#define PRG_ROM_BYTE 4
#define CHR_ROM_BYTE 5
#define PRG_RAM_BYTE 8

#define FOUR_SCREEN_MIRRORING 0x08
#define HORIZONTAL_MIRRORING 0x00
#define VERTICAL_MIRRORING 0x01

#define BATTERY_BACKED_SRAM 0x02
#define TRAINER 0x04

#define MAPPER_MASK 0xF0

#define PRG_ROM_PAGE_SIZE 16384 // 16kb
#define CHR_ROM_PAGE_SIZE 8192  // 8kb
#define PRG_RAM_PAGE_SIZE 8192  // 8kb

#include <bitset>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class RomReader {
private:
    ifstream::pos_type fileSize;
    char header[HEADER_SIZE];
    int prgRomBytes;
    int chrRomBytes;
    int prgRamBytes;
    
    char *prgRom;
    char *chrRom;
    char *prgRam;
    
public:
    RomReader(string filename);
    
    char *getPrgRom();
    char *getChrRom();
    
    void printDebugInfo();
};

#endif /* defined(__Emulator__RomReader__) */

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

#include <fstream>
#include <string>

class RomReader {
private:
    std::ifstream::pos_type fileSize;
    char header[HEADER_SIZE];
    int prg_rom_bytes;
    int chr_rom_bytes;
    int prg_ram_bytes;

    char *prg_rom;
    char *chr_rom;
    char *prg_ram;

public:
    RomReader(std::string filename);
    ~RomReader();

    char *get_prg_rom();
    char *get_chr_rom();

    void printDebugInfo();
};

#endif /* defined(__Emulator__RomReader__) */

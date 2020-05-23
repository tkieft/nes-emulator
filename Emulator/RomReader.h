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

#define PRG_ROM_PAGE_SIZE 16384  // 16kb
#define CHR_ROM_PAGE_SIZE 8192   // 8kb
#define PRG_RAM_PAGE_SIZE 8192   // 8kb

#include <memory>
#include <string>

#include "defines.h"

class RomReader {
 private:
  long fileSize;
  byte header[HEADER_SIZE];

  long prg_rom_bytes;
  long chr_rom_bytes;
  long prg_ram_bytes;

  std::unique_ptr<byte[]> prg_rom;
  std::unique_ptr<byte[]> chr_rom;
  std::unique_ptr<byte[]> prg_ram;

 public:
  RomReader(std::string filename);

  std::unique_ptr<byte[]>&& get_prg_rom();
  std::unique_ptr<byte[]>&& get_chr_rom();

  void printDebugInfo();
};

#endif /* defined(__Emulator__RomReader__) */

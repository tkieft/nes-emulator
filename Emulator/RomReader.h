//
//  RomReader.h
//  Emulator
//
//  Created by Tyler Kieft on 9/25/12.
//
//

#ifndef __Emulator__RomReader__
#define __Emulator__RomReader__

#include <memory>
#include <string>

#include "defines.h"

class RomReader {
 private:
  static const int kROMHeaderSize = 16;

  long fileSize;
  byte header[kROMHeaderSize];

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

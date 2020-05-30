//
//  RomReader.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/25/12.
//
//

#include "RomReader.h"

#include <bitset>
#include <fstream>
#include <iostream>

static const int kPRGROMByte = 4;
static const int kCHRROMByte = 5;
static const int kPRGRAMByte = 8;

static const int kFourScreenMirroring = 0x08;
static const int kHorizontalMirroring = 0x00;
static const int kVerticalMirroring = 0x01;

static const int kBatteryBackedSRAM = 0x02;
static const int kTrainer = 0x04;

static const int kMapperMask = 0xF0;

static const int kPRGROMPageSize = 16384;  // 16kb
static const int kCHRROMPageSize = 8192;   // 8kb
static const int kPRGRAMPageSize = 8192;   // 8kb

RomReader::RomReader(std::string filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

  if (file.is_open()) {
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(header), kROMHeaderSize);

    if (!(header[0] == 'N' && header[1] == 'E' && header[2] == 'S' &&
          header[3] == '\x1A')) {
      file.close();
      throw "This is not a valid NES ROM!";
    }

    prg_rom_bytes = header[kPRGROMByte] * kPRGROMPageSize;
    chr_rom_bytes = header[kCHRROMByte] * kCHRROMPageSize;
    prg_ram_bytes = header[kPRGRAMByte] * kPRGRAMPageSize;

    if (!prg_ram_bytes) {
      // This can be 0 in an old value of the .NES file format; assume 1 page if
      // so
      prg_ram_bytes = kPRGRAMPageSize;
    }

    // Error checking
    if (!prg_rom_bytes) {
      throw "Program ROM bytes = 0";
    }

    if (kROMHeaderSize + prg_rom_bytes + chr_rom_bytes != fileSize) {
      throw "Inconsistent file size";
    }

    prg_rom = std::make_unique<byte[]>(static_cast<size_t>(prg_rom_bytes));
    file.read(reinterpret_cast<char*>(prg_rom.get()), prg_rom_bytes);

    chr_rom = nullptr;
    if (chr_rom_bytes) {
      chr_rom = std::make_unique<byte[]>(static_cast<size_t>(chr_rom_bytes));
      file.read(reinterpret_cast<char*>(chr_rom.get()), chr_rom_bytes);
    }

    prg_ram = std::make_unique<byte[]>(static_cast<size_t>(prg_ram_bytes));
    file.read(reinterpret_cast<char*>(prg_ram.get()), prg_ram_bytes);

    file.close();
  } else {
    throw "Could not open file for reading.";
  }
}

std::unique_ptr<byte[]>&& RomReader::get_prg_rom() {
  return std::move(prg_rom);
}

std::unique_ptr<byte[]>&& RomReader::get_chr_rom() {
  return std::move(chr_rom);
}

void RomReader::printDebugInfo() {
  std::bitset<8> flags6(header[6]);
  std::bitset<8> flags7(header[7]);
  std::bitset<8> flags9(header[9]);
  std::bitset<8> flags10(header[10]);

  std::cout << "File size is " << fileSize << " bytes." << std::endl
            << std::endl

            << header[0] << header[1] << header[2] << std::endl
            << "Size of PRG ROM: " << prg_rom_bytes << " bytes" << std::endl
            << "Size of CHR ROM: " << chr_rom_bytes << " bytes" << std::endl
            << "Size of PRG RAM: " << prg_ram_bytes << " bytes" << std::endl
            << std::endl

            << "Flags 6: " << flags6 << std::endl
            << "Flags 7: " << flags7 << std::endl
            << "Flags 9: " << flags9 << std::endl
            << "Flags 10: " << flags10 << std::endl;

  // Mirroring type
  std::cout << std::endl << "Mirroring Type: ";
  if (header[6] & kFourScreenMirroring) {
    std::cout << "Four-screen" << std::endl;
  } else if (header[6] & kVerticalMirroring) {
    std::cout << "Vertical" << std::endl;
  } else if (header[6] & kHorizontalMirroring) {
    std::cout << "Horizontal" << std::endl;
  }

  if (header[6] & kBatteryBackedSRAM) {
    std::cout << "Battery backed SRAM in CPU $6000-$7FFF" << std::endl;
  }

  if (header[6] & kTrainer) {
    std::cout << "512 byte trainer at $7000-$71FF" << std::endl;
  }

  int mapper = ((header[6] & kMapperMask) >> 4) | (header[7] & kMapperMask);
  if (mapper) {
    std::cout << "Memory Mapper: " << mapper << std::endl;
  }

  if (header[7] || header[9] || header[10]) {
    std::cout << "Unimplemented features in flags 7-10!";
  }
}

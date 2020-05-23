//
//  PPU.h
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#ifndef __Emulator__PPU__
#define __Emulator__PPU__

#include <memory>

#include "SDLRenderer.h"
#include "defines.h"

const dbyte kVRAMSize = 0x4000;
const dbyte kPatternTableSize = 0x2000;
const dbyte kSprRAMSize = 0x100;
const dbyte kPaletteTableStart = 0x3F00;
const dbyte kPaletteTableSpriteOffset = 16;

class PPU {
  friend class SDLRenderer;

 private:
  std::unique_ptr<SDLRenderer> renderer;

  byte vram[kVRAMSize];
  byte spr_ram[kSprRAMSize];

  byte control_1;
  byte control_2;
  byte status;
  byte sprite_memory_address;

  byte read_buffer;

  // PPU Scroll Registers
  byte regFV;  // Fine vertical scroll latch (3 bits)
  byte regV;   // Vertical name table selection latch (1 bit)
  byte regH;   // Horizontal name table selection latch (1 bit)
  byte regVT;  // Vertical tile index latch (5 bits)
  byte regHT;  // Horizontal tile index latch (5 bits)
  byte regFH;  // Fine horizontal scroll latch (3 bits)
  byte regS;   // Playfield pattern selection table latch

  // PPU Scroll counters
  byte cntFV;
  byte cntV;
  byte cntH;
  byte cntVT;
  byte cntHT;

  bool first_write;

  dbyte calculate_effective_address(dbyte address);
  byte read_memory(dbyte address);
  void store_memory(dbyte address, byte word);

  void reset_vblank_flag();

  void reset_sprite_0_flag();
  void set_sprite_0_flag();

  void reset_more_than_8_sprites_flag();
  void set_more_than_8_sprites_flag();

  bool enable_background();
  bool enable_sprites();
  bool use_8x16_sprites();
  dbyte sprite_pattern_table_address();

  dbyte vram_address();
  dbyte nametable_address();
  dbyte attributetable_address();
  dbyte patterntable_address();

  byte palette_select_bits();

  void increment_scroll_counters();
  void increment_horizontal_scroll_counter();
  void increment_vertical_scroll_counter();
  void update_scroll_counters_from_registers();

  bool is_screen_enabled();

 public:
  PPU();

  bool render_scanline(int scanline);
  void set_chr_rom(std::unique_ptr<byte[]> chr_rom);

  byte read_status();
  byte read_control_1();
  void write_control_1(byte value);
  byte read_control_2();
  void write_control_2(byte value);

  void write_spr_ram(byte* start);  // DMA
  void set_sprite_memory_address(byte value);
  byte read_sprite_data();
  void write_sprite_data(byte value);

  void write_scroll_register(byte value);

  void write_vram_address(byte value);
  byte read_vram_data();
  void write_vram_data(byte value);
};

#endif /* defined(__Emulator__PPU__) */

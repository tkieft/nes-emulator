/*
 *  Emulator.cpp
 *  Emulator
 *
 *  Created by Tyler Kieft on 9/24/12.
 *
 *
 */

#include "Emulator.h"

#include "ControllerPad.h"
#include "RomReader.h"

Emulator::Emulator()
    : processor(std::make_unique<Processor>(&ppu, &controller_pad)) {}

void Emulator::load_rom(std::string filename) {
  RomReader reader(filename);
  ppu.set_chr_rom(reader.get_chr_rom());
  processor->set_prg_rom(reader.get_prg_rom());
  processor->reset();
}

void Emulator::emulate_frame() {
  // The PPU renders 262 scanlines. The CPU does ~113 clock cycles per scanline.
  //
  // TODO: Make this more accurate?
  //
  // VINT: Pre-render 20 blank scanlines
  // 1 Dummy scanline
  // 240 Picture scanlines
  // 1 Dummy scanline -> VINT set afterwards
  int clock = 0;
  for (int scanline = 0; scanline <= 261; scanline++) {
    while (clock < 113) {
      clock += processor->execute();
    }

    if (ppu.render_scanline(scanline)) {
      processor->non_maskable_interrupt();
      // NMI takes 7 cycles to execute
      clock += 7;
    }

    clock -= 113;
  }
}

bool Emulator::handle_key_down(SDL_Keysym sym) {
  return controller_pad.record_key_down(sym);
}

bool Emulator::handle_key_up(SDL_Keysym sym) {
  return controller_pad.record_key_up(sym);
}

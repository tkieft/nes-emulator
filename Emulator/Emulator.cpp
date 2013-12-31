/*
 *  Emulator.cpp
 *  Emulator
 *
 *  Created by Tyler Kieft on 9/24/12.
 *
 *
 */

#include "Emulator.h"
#include "RomReader.h"
#include "ControllerPad.h"

Emulator::Emulator() {
    ppu = new PPU();
    controller_pad = new ControllerPad();
    processor = new Processor(ppu, controller_pad);
}

Emulator::~Emulator() {
    delete ppu;
    delete processor;
    delete controller_pad;
}

void Emulator::load_rom(std::string filename) {
    RomReader reader(filename);

    if (reader.get_chr_rom() != NULL) {
        ppu->set_chr_rom((uint8_t *)reader.get_chr_rom());
    }
    
    processor->set_prg_rom((uint8_t *)reader.get_prg_rom());
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
    for (int scanline = 0; scanline <= 261; scanline++) {
        int clock = 0;
        
        while (clock < 113) {
            processor->execute();
            clock += 3; // TODO: Holy shit accurate clock cycles.
        }
        
        if (ppu->render_scanline(scanline)) {
            processor->non_maskable_interrupt();
        }
    }
}

bool Emulator::handle_key_down(SDLKey sym) {
    return controller_pad->record_key_down(sym);
}

bool Emulator::handle_key_up(SDLKey sym) {
    return controller_pad->record_key_up(sym);
}
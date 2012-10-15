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
    ppu->set_chr_rom((uint8_t *)reader.get_chr_rom());
    processor->set_prg_rom((uint8_t *)reader.get_prg_rom());
    processor->reset();
}

void Emulator::emulate_frame() {
    int clock = 0;
    
    while (clock < 29830) {
        processor->execute();
        clock += 5;
    }
    
    if (ppu->render()) {
        processor->non_maskable_interrupt();
    }
}

void Emulator::resize(int width, int height) {
	ppu->resize(width, height);
}

bool Emulator::handle_key_down(unsigned short key_code) {
    return controller_pad->record_key_down(key_code);
}

bool Emulator::handle_key_up(unsigned short key_code) {
    return controller_pad->record_key_up(key_code);
}
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

Emulator::Emulator() {
    ppu = new PPU;
    processor = new Processor(ppu);
}

Emulator::~Emulator() {
    delete ppu;
    delete processor;
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

void Emulator::resize(GLuint width, GLuint height) {
	ppu->resize(width, height);
}
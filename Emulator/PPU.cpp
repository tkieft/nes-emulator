//
//  PPU.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#include <string.h>

#include "PPU.h"

PPU::PPU() {
    control_1 = 0;
    control_2 = 0;
    status = PPU_STATUS_VBLANK_MASK;
    
    // set toggle
    first_write = true;
}
void PPU::set_chr_rom(uint8_t *chr_rom) {
    memcpy(vram, chr_rom, PATTERN_TABLE_SIZE);
}

/** PPU MEMORY **/

uint8_t PPU::read_memory(uint16_t address) {
    // VRAM is mirrored 4x
    return vram[address & 0x07FF];
}

void PPU::store_memory(uint16_t address, uint8_t word) {
    // VRAM is mirrored 4x
    vram[address & 0x07FF] = word;
}

/** REGISTER READS AND WRITES */

uint8_t PPU::read_status() {
    uint8_t temp = status;
    return temp;
}
uint8_t PPU::read_control_1() {
    return control_1;
}
void PPU::write_control_1(uint8_t value) {
    control_1 = value;
}
uint8_t PPU::read_control_2() {
    return control_2;
}
void PPU::write_control_2(uint8_t value) {
    control_2 = value;
}

void PPU::write_spr_ram(char* start) {
    memcpy(spr_ram, start, 0x100);
}

void PPU::set_sprite_memory_address(uint8_t value) {
    sprite_memory_address = value;
}

void PPU::set_sprite_data(uint8_t value) {
    spr_ram[sprite_memory_address++] = value;
}

uint8_t PPU::read_sprite_data() {
    return spr_ram[sprite_memory_address++];
}
//
//  PPU.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#include <iomanip>
#include <iostream>
#include <string.h>

#include "PPU.h"

using namespace std;

PPU::PPU() {
    control_1 = 0;
    control_2 = 0;
    status = 0;
    
    // set toggle
    first_write = true;
    first_read = true;
    
    for (int i = 0; i < VRAM_SIZE; i++) {
        vram[i] = 0;
    }
}
void PPU::set_chr_rom(uint8_t *chr_rom) {
    memcpy(vram, chr_rom, PATTERN_TABLE_SIZE);
}

bool PPU::render_screen() {
    if ((control_2 & SCREEN_ENABLE_MASK) == SCREEN_ENABLE) {
        
        // 30 rows, 32 cols
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 32; j++) {
                uint8_t character = read_memory(0x2000 + i * 32 + j);
                if (character != 0x24)
                    cout << setw(2) << setfill('0') << hex << (int)character << " ";
                else
                    cout << "   ";
            }
            
            cout << endl;
        }
        
        cout << endl << endl << endl;
    }
    
    status |= PPU_STATUS_VBLANK_MASK;
    
    return control_1 & VBLANK_INTERRUPT_ENABLE_MASK;
}

/** PPU MEMORY **/
uint16_t PPU::calculate_effective_address(uint16_t address) {
    if (address < 0x2000) {
        return address;
    } else if (address >= 0x2000 && address < 0x3000) {
        return address & 0x27FF;
    } else {
        return address;
    }
}

uint8_t PPU::read_memory(uint16_t address) {
    return vram[calculate_effective_address(address)];
}

void PPU::store_memory(uint16_t address, uint8_t word) {
    vram[calculate_effective_address(address)] = word;
}

/** VBLANK **/
void PPU::reset_vblank_flag() {
    status &= ~PPU_STATUS_VBLANK_MASK;
}


/** REGISTER READS AND WRITES */

uint8_t PPU::read_status() {
    uint8_t temp = status;
    reset_vblank_flag();    // reset vblank flag
    first_write = true;     // reset toggle
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
void PPU::write_sprite_data(uint8_t value) {
    spr_ram[sprite_memory_address++] = value;
}
uint8_t PPU::read_sprite_data() {
    return spr_ram[sprite_memory_address++];
}
void PPU::write_scroll_register(uint8_t value) {
    if (first_write) {
        vertical_scroll = value;
    } else {
        horizontal_scroll = value;
    }
    
    first_write = !first_write;
}
void PPU::write_vram_address(uint8_t value) {
    if (first_write) {
        vram_address = (uint16_t)value << 8;
    } else {
        vram_address |= value;
    }
    
    first_read = true;
    first_write = !first_write;
}

uint8_t PPU::read_vram_data() {
    if (first_read) {
        return read_memory(vram_address);
    }
    
    return read_memory(vram_address++);
}

void PPU::write_vram_data(uint8_t value) {
    store_memory(vram_address, value);
    vram_address += ((control_1 & VERTICAL_WRITE_MASK) == VERTICAL_WRITE_ON ? 32 : 1);
}
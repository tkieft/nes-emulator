//
//  PPU.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#include <string.h>

#include "PPU.h"

#include "TerminalRenderer.h"
#include "OpenGLRenderer.h"

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
    
    renderer = new OpenGLRenderer(this);
}

PPU::~PPU() {
    delete renderer;
}

bool PPU::render() {
    if ((control_2 & SCREEN_ENABLE_MASK) == SCREEN_ENABLE) {
        renderer->render();
    }
    
    status |= PPU_STATUS_VBLANK_MASK;
    
    return control_1 & VBLANK_INTERRUPT_ENABLE_MASK;
}

void PPU::set_chr_rom(uint8_t *chr_rom) {
    memcpy(vram, chr_rom, PATTERN_TABLE_SIZE);
}

void PPU::resize(GLuint width, GLuint height) {
	glViewport(0, 0, width, height);
	
	//m_viewWidth = width;
	//m_viewHeight = height;
}

/** PPU MEMORY **/
uint16_t PPU::calculate_effective_address(uint16_t address) {
    address &= 0x3FFF;
    
    if (address >= 0x2000 && address <= 0x3000) {
        address &= 0x27FF; // controlled by vertical/horiz mirroring
    } else if (address >= 0x3F00) {
        address &= 0x3F1F;
    }
    
    return address;
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
    memcpy(spr_ram, start, SPR_RAM_SIZE);
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
        first_read = false;
        return read_memory(vram_address);
    }
    
    return read_memory(vram_address++);
}

void PPU::write_vram_data(uint8_t value) {
    store_memory(vram_address, value);
    vram_address += ((control_1 & VERTICAL_WRITE_MASK) == VERTICAL_WRITE_ON ? 32 : 1);
}
//
//  PPU.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#include <iostream>
#include <string.h>

#include "PPU.h"

PPU::PPU() {
    control_1 = 0;
    control_2 = 0;
    status = 0;

    // set toggle
    first_write = true;

    for (int i = 0; i < VRAM_SIZE; i++) {
        vram[i] = 0;
    }

    renderer = new SDLRenderer(this);
}

PPU::~PPU() {
    delete renderer;
}

bool PPU::render_scanline(int scanline) {
    if (scanline == 0) {
        reset_sprite_0_flag();
    }

    if (scanline == 20 && is_screen_enabled()) {
        // This should happen at clock cycle 256!
        update_scroll_counters_from_registers();
    }

    // These are the actual drawing scanlines
    if (scanline >= 21 && scanline <= 260 && is_screen_enabled()) {
        renderer->render_scanline(scanline - 21);

        // H & HT counters are updated at the end of hblank
        cntH = regH;
        cntHT = regHT;
    }

    if (scanline == 261) {
        // Set the VBlank flag in the status register
        status |= PPU_STATUS_VBLANK_MASK;

        // If VBlank interrupts are enabled, return true which will generate an interrupt
        return control_1 & VBLANK_INTERRUPT_ENABLE_MASK;
    }

    return false;
}


//
// PPU Memory Map
//
// --------------------------------------- $4000
// Mirrors $3F00-$3F1F
// --------------------------------------- $3F20
// Sprite Palette
// --------------------------------------- $3F10
// Image Palette
// --------------------------------------- $3F00
// Mirrors $2000 - $2EFF
// --------------------------------------- $3000
// Attribute Table 3
// --------------------------------------- $2FC0
// Name Table 3 (32x30 tiles)
// --------------------------------------- $2C00
// Attribute Table 2
// --------------------------------------- $2BC0
// Name Table 2 (32x30 tiles)
// --------------------------------------- $2800
// Attribute Table 1
// --------------------------------------- $27C0
// Name Table 1 (32x30 tiles)
// --------------------------------------- $2400
// Attribute Table 0
// --------------------------------------- $23C0
// Name Table 0 (32x30 tiles)
// --------------------------------------- $2000
// Pattern Table 1 (256x2x8, may be VROM)
// --------------------------------------- $1000
// Pattern Table 0 (256x2x8, may be VROM)
// --------------------------------------- $0000
//

//
// The Pattern Table may come from VROM.
//
void PPU::set_chr_rom(uint8_t *chr_rom) {
    memcpy(vram, chr_rom, PATTERN_TABLE_SIZE);
}

uint16_t PPU::calculate_effective_address(uint16_t address) {
    // Only consider the least significant 14 bits to form the address
    address &= 0x3FFF;

    if (address < 0x2000) {
        return address;
    } else if (address < 0x3000) {
        // There is only physical space for two name & attribute tables in the PPU memory
        // Mirroring bits in the cartridge control how the 4 virtual tables are mapped
        //
        // TODO: Support mirroring, right now hard-coded to vertical
        return address & 0x27FF;
    } else if (address < 0x3F00) {
        // 0x3000 - 0x3EFF is a mirror of 0x2000 - 0x2EFF
        return address - 0x1000;
    } else {
        // Image / Sprite palette are mirrored 8 times from 0x3F00 to 0x4000
        address &= 0x3F1F;

        // 0x3F1x is a mirror of 0x3F0x
        if ((address & 0x03) == 0) {
            address &= 0x3F0F;
        }
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

/** SPRITE HIT **/
void PPU::reset_sprite_0_flag() {
    status &= ~PPU_STATUS_SPRITE_0_HIT_MASK;
}

void PPU::set_sprite_0_flag() {
    status |= PPU_STATUS_SPRITE_0_HIT_MASK;
}

/** MORE THAN 8 SPRITES ON A SCANLINE **/
void PPU::reset_more_than_8_sprites_flag() {
    status &= ~PPU_STATUS_MORE_THAN_8_SPRITES_HIT_MASK;
}

void PPU::set_more_than_8_sprites_flag() {
    status |= PPU_STATUS_MORE_THAN_8_SPRITES_HIT_MASK;
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

    regH = (value & NAME_TABLE_X_SCROLL_MASK) >> NAME_TABLE_X_SCROLL_BIT;
    regV = (value & NAME_TABLE_Y_SCROLL_MASK) >> NAME_TABLE_Y_SCROLL_BIT;
    regS = (value & BACKGROUND_PATTERN_TABLE_ADDRESS_MASK) >> BACKGROUND_PATTERN_TABLE_ADDRESS_BIT;
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
        regFH = value & 0x07;
        regHT = (value & 0xF8) >> 3;
    } else {
        regFV = value & 0x07;
        regVT = (value & 0xF8) >> 3;
    }

    first_write = !first_write;
}
void PPU::write_vram_address(uint8_t value) {
    if (first_write) {
        regVT = (regVT & 0x07) | ((value & 0x03) << 3);
        regH = (value & 0x04) >> 2;
        regV = (value & 0x08) >> 3;
        regFV = (value & 0x30) >> 4;
    } else {
        regHT = (value & 0x1F);
        regVT = (regVT & 0x18) | ((value & 0xE0) >> 5);

        update_scroll_counters_from_registers();
    }

    first_write = !first_write;
}

uint8_t PPU::read_vram_data() {
    uint8_t result;

    if (vram_address() >= PALETTE_TABLE_START) {
        result = read_memory(vram_address());
    } else {
        result = read_buffer;
        read_buffer = read_memory(vram_address());
    }

    increment_scroll_counters();
    return result;
}

void PPU::write_vram_data(uint8_t value) {
    store_memory(vram_address(), value);
    increment_scroll_counters();
}

uint16_t PPU::vram_address() {
    uint16_t address = (uint16_t)cntHT;
    address |= ((uint16_t)cntVT) << 5;
    address |= ((uint16_t)cntH)  << 10;
    address |= ((uint16_t)cntV)  << 11;
    address |= ((uint16_t)cntFV & 0x03) << 12;
    return address;
}

uint16_t PPU::nametable_address() {
    uint16_t address = 0x2000;
    address |= (uint16_t)cntHT;
    address |= (uint16_t)cntVT << 5;
    address |= (uint16_t)cntH  << 10;
    address |= (uint16_t)cntV  << 11;
    return address;
}

// Use the attribute table to determine which palette to use. The byte in the name table determines
// the actual color to use in the palette. Returns the two high order bits of the palette entry.
uint8_t PPU::palette_select_bits() {
    int attr_byte = read_memory(attributetable_address());

    // Figure out which two bits to take
    int bits_offset = ((cntVT & 0x02) << 1) | (cntHT & 0x02);

    // Mask and shift
    return (attr_byte & (0x03 << (bits_offset))) >> bits_offset;
}

uint16_t PPU::attributetable_address() {
    uint16_t address = 0x23C0;
    address |= ((uint16_t)cntHT & 0x1C) >> 2;
    address |= ((uint16_t)cntVT & 0x1C) << 1;
    address |= (uint16_t)cntH  << 10;
    address |= (uint16_t)cntV  << 11;
    return address;
}

uint16_t PPU::patterntable_address() {
    uint8_t pattern_index = read_memory(nametable_address());

    uint16_t address = (uint16_t)regS << 12;
    address |= (uint16_t)pattern_index << 4;
    address |= cntFV;
    return address;
}

// For use during R/W of $2007
void PPU::increment_scroll_counters() {
    bool vertical_write = ((control_1 & VERTICAL_WRITE_MASK) == VERTICAL_WRITE_ON);

    if (!vertical_write) {
        if (++cntHT == 0x20) {
            cntHT = 0;
            cntVT++;
        }
    } else {
        cntVT++;
    }

    if (cntVT == 0x20) {
        cntVT = 0;

        if (++cntH == 0x02) {
            cntH = 0;

            if (++cntV == 0x02) {
                cntV = 0;

                if (++cntFV == 0x08) {
                    cntFV = 0;
                }
            }
        }
    }
}

// For use during rendering
void PPU::increment_horizontal_scroll_counter() {
    if (++cntHT == 0x20) {
        cntHT = 0;

        if (++cntH == 0x02) {
            cntH = 0;
        }
    }
}

void PPU::increment_vertical_scroll_counter() {
    if (++cntFV == 0x08) {
        cntFV = 0;

        if (++cntVT == 30) { // Is this correct??
            cntVT = 0;

            if (++cntV == 0x02) {
                cntV = 0;
            }
        }
    }
}

void PPU::update_scroll_counters_from_registers() {
    cntFV = regFV;
    cntV = regV;
    cntH = regH;
    cntVT = regVT;
    cntHT = regHT;
}

bool PPU::is_screen_enabled() {
    return (control_2 & BACKGROUND_ENABLE_MASK) == BACKGROUND_ENABLE ||
           (control_2 & SPRITES_ENABLE_MASK) == SPRITES_ENABLE;
}

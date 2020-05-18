//
//  PPU.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#include "PPU.h"

// PPU CONTROL REGISTER 1
const int kNameTableXScrollBit = 0;
const int kNameTableYScrollBit = 1;
const int kVerticalWriteBit = 2;
const int kSpritePatternTableAddressBit = 3;
const int kBackgroundPatternTableAddressBit = 4;
const int kSpriteSizeBit = 5;
const int kVBlankInterruptEnableBit = 7;

const int kNameTableXScrollMask = 1 << kNameTableXScrollBit;
const int kNameTableYScrollMask = 1 << kNameTableYScrollBit;
const int kVerticalWriteMask = 1 << kVerticalWriteBit;
const int kSpritePatternTableAddressMask = 1 << kSpritePatternTableAddressBit;
const int kBackgroundPatternTableAddressMask = 1 << kBackgroundPatternTableAddressBit;
const int kSpriteSizeMask = 1 << kSpriteSizeBit;
const int kVBlankInterruptEnableMask = 1 << kVBlankInterruptEnableBit;

// For 8x8 sprites only
#define SPRITE_PATTERN_TABLE_ADDRESS_0000 0x00
#define SPRITE_PATTERN_TABLE_ADDRESS_1000 0x08

// compare with regs
#define REGS_BACKGROUND_PATTERN_TABLE_ADDRESS_0000 0x00
#define REGS_BACKGROUND_PATTERN_TABLE_ADDRESS_1000 0x01

#define SPRITE_SIZE_8x8 0x00
#define SPRITE_SIZE_8x16 0x20

// PPU CONTROL REGISTER 2
const int kBackgroundEnableBit = 3;
const int kSpritesEnableBit = 4;

const int kBackgroundEnableMask = 1 << kBackgroundEnableBit;
const int kSpritesEnableMask = 1 << kSpritesEnableBit;

// PPU STATUS REGISTER
const int kPPUStatusSpriteOverflowBit = 5;  // ~= More than 8 sprites on a scanline
const int kPPUStatusSprite0Bit = 6;
const int kPPUStatusVBlankBit = 7;

const int kPPUStatusSpriteOverflowMask = 1 << kPPUStatusSpriteOverflowBit;
const int kPPUStatusSprite0Mask = 1 << kPPUStatusSprite0Bit;
const int kPPUStatusVBlankMask = 1 << kPPUStatusVBlankBit;

PPU::PPU() : control_1(0),
             control_2(0),
             status(0),
             vram(),
             first_write(true),  // set toggle
             renderer(std::make_unique<SDLRenderer>(this)) {}

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
        status |= kPPUStatusVBlankMask;

        // If VBlank interrupts are enabled, return true which will generate an interrupt
        return control_1 & kVBlankInterruptEnableMask;
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
void PPU::set_chr_rom(std::unique_ptr<byte[]> chr_rom) {
    ::memcpy(vram, chr_rom.get(), kPatternTableSize);
}

dbyte PPU::calculate_effective_address(dbyte address) {
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

byte PPU::read_memory(dbyte address) {
    return vram[calculate_effective_address(address)];
}

void PPU::store_memory(dbyte address, byte word) {
    vram[calculate_effective_address(address)] = word;
}

/** VBLANK **/
void PPU::reset_vblank_flag() {
    status &= ~kPPUStatusVBlankMask;
}

/** SPRITE HIT **/
void PPU::reset_sprite_0_flag() {
    status &= ~kPPUStatusSprite0Mask;
}

void PPU::set_sprite_0_flag() {
    status |= kPPUStatusSprite0Mask;
}

/** MORE THAN 8 SPRITES ON A SCANLINE **/
void PPU::reset_more_than_8_sprites_flag() {
    status &= ~kPPUStatusSpriteOverflowMask;
}

void PPU::set_more_than_8_sprites_flag() {
    status |= kPPUStatusSpriteOverflowMask;
}

bool PPU::enable_background() {
    return control_2 & kBackgroundEnableMask;
}

bool PPU::enable_sprites() {
    return control_2 & kSpritesEnableMask;
}

bool PPU::use_8x16_sprites() {
    return control_2 & kSpriteSizeMask;
}

dbyte PPU::sprite_pattern_table_address() {
    return control_1 & kSpritePatternTableAddressMask ? 0x1000 : 0;
}

/** REGISTER READS AND WRITES */

byte PPU::read_status() {
    byte result = status;
    reset_vblank_flag();    // reset vblank flag
    first_write = true;     // reset toggle
    return result;
}
byte PPU::read_control_1() {
    return control_1;
}
void PPU::write_control_1(byte value) {
    control_1 = value;

    regH = (value & kNameTableXScrollMask) >> kNameTableXScrollBit;
    regV = (value & kNameTableYScrollMask) >> kNameTableYScrollBit;
    regS = (value & kBackgroundPatternTableAddressMask) >> kBackgroundPatternTableAddressBit;
}

byte PPU::read_control_2() {
    return control_2;
}
void PPU::write_control_2(byte value) {
    control_2 = value;
}
void PPU::write_spr_ram(byte* start) {
    ::memcpy(spr_ram, start, kSprRAMSize);
}
void PPU::set_sprite_memory_address(byte value) {
    sprite_memory_address = value;
}
void PPU::write_sprite_data(byte value) {
    spr_ram[sprite_memory_address++] = value;
}
byte PPU::read_sprite_data() {
    return spr_ram[sprite_memory_address++];
}
void PPU::write_scroll_register(byte value) {
    if (first_write) {
        regFH = value & 0x07;
        regHT = (value & 0xF8) >> 3;
    } else {
        regFV = value & 0x07;
        regVT = (value & 0xF8) >> 3;
    }

    first_write = !first_write;
}
void PPU::write_vram_address(byte value) {
    if (first_write) {
        regVT = (regVT & 0x07) | (value & 0x03) << 3;
        regH = (value & 0x04) >> 2;
        regV = (value & 0x08) >> 3;
        regFV = (value & 0x30) >> 4;
    } else {
        regHT = (value & 0x1F);
        regVT = (regVT & 0x18) | (value & 0xE0) >> 5;

        update_scroll_counters_from_registers();
    }

    first_write = !first_write;
}

byte PPU::read_vram_data() {
    byte result;

    if (vram_address() >= kPaletteTableStart) {
        result = read_memory(vram_address());
    } else {
        result = read_buffer;
        read_buffer = read_memory(vram_address());
    }

    increment_scroll_counters();
    return result;
}

void PPU::write_vram_data(byte value) {
    store_memory(vram_address(), value);
    increment_scroll_counters();
}

dbyte PPU::vram_address() {
    return cntHT |
           cntVT << 5 |
           cntH  << 10 |
           cntV  << 11 |
           (cntFV & 0x03) << 12;
}

dbyte PPU::nametable_address() {
    return 0x2000 |
           cntHT  |
           cntVT << 5 |
           cntH  << 10 |
           cntV  << 11;
}

// Use the attribute table to determine which palette to use. The byte in the name table determines
// the actual color to use in the palette. Returns the two high order bits of the palette entry.
byte PPU::palette_select_bits() {
    byte attr_byte = read_memory(attributetable_address());

    // Figurex out which two bits to take
    int bits_offset = ((cntVT & 0x02) << 1) | (cntHT & 0x02);

    // Mask and shift
    return (attr_byte & (0x03 << (bits_offset))) >> bits_offset;
}

dbyte PPU::attributetable_address() {
    return 0x23C0 |
           (cntHT & 0x1C) >> 2 |
           (cntVT & 0x1C) << 1 |
           cntH  << 10 |
           cntV  << 11;
}

dbyte PPU::patterntable_address() {
    byte pattern_index = read_memory(nametable_address());

    return regS << 12 |
           pattern_index << 4 |
           cntFV;
}

// For use during R/W of $2007
void PPU::increment_scroll_counters() {
    bool vertical_write = control_1 & kVerticalWriteMask;

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
    return control_2 & kBackgroundEnableMask ||
           control_2 & kSpritesEnableMask;
}

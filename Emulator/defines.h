//
//  defines.h
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#ifndef Emulator_defines_h
#define Emulator_defines_h

#include <stdint.h>

#define FPS 60
#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define SCREEN_BPP 32

#define VRAM_SIZE 0x4000
#define PATTERN_TABLE_SIZE 0x2000
#define SPR_RAM_SIZE 0x100
#define PALETTE_TABLE_START 0x3F00
#define PALETTE_TABLE_SPRITE_OFFSET 16

/*** PROCESSOR STATUS REGISTER ***/

#define CARRY_BIT       0       // Set when the result of the computation is < 0 or > 255 (unsigned)
#define CARRY_MASK      0x01
#define ZERO_BIT        1       // Set when the result of the computation is 0
#define ZERO_MASK       0x02
#define INTERRUPT_BIT   2
#define INTERRUPT_MASK  0x04
#define DECIMAL_BIT     3       // unused in the NES version of the 6502
#define DECIMAL_MASK    0x08
#define BREAK_BIT       4
#define BREAK_MASK      0x10
#define UNUSED_BIT      5
#define UNUSED_MASK     0x20
#define OVERFLOW_BIT    6       // Set when there is a 2's complement overflow
#define OVERFLOW_MASK   0x40
#define SIGN_BIT        7       // Indicates the sign of the result (set when negative)
#define SIGN_MASK       0x80

#define CPU_RAM_SIZE    2048
#define SRAM_SIZE       8192

/*** PPU CONTROL REGISTER 1 ***/
#define VERTICAL_WRITE_MASK          0x04
#define SPRITE_PATTERN_TABLE_ADDRESS_MASK 0x08
#define BACKGROUND_PATTERN_TABLE_ADDRESS_MASK 0x10
#define SPRITE_SIZE_MASK 0x20
#define VBLANK_INTERRUPT_ENABLE_MASK 0x80

#define VERTICAL_WRITE_ON 0x04
#define VERTICAL_WRITE_OFF 0x00

// For 8x8 sprites only
#define SPRITE_PATTERN_TABLE_ADDRESS_0000 0x00
#define SPRITE_PATTERN_TABLE_ADDRESS_1000 0x08

#define BACKGROUND_PATTERN_TABLE_ADDRESS_0000 0x00
#define BACKGROUND_PATTERN_TABLE_ADDRESS_1000 0x10

#define SPRITE_SIZE_8x8 0x00
#define SPRITE_SIZE_8x16 0x20

#define VBLANK_INTERRUPT_ENABLE 0x80
#define VBLANK_INTERRUPT_DISABLE 0x00

/*** PPU CONTROL REGISTER 2 ***/
#define BACKGROUND_ENABLE_MASK 0x08
#define SPRITES_ENABLE_MASK 0x10

#define BACKGROUND_DISABLE 0x00
#define BACKGROUND_ENABLE 0x08

#define SPRITES_DISABLE 0x00
#define SPRITES_ENABLE 0x10

/*** PPU STATUS ***/
#define PPU_STATUS_MORE_THAN_8_SPRITES_HIT_MASK 0x20
#define PPU_STATUS_SPRITE_0_HIT_MASK 0x40
#define PPU_STATUS_VBLANK_MASK 0x80

#define PPU_STATUS_SPRITE_0_HIT     0x40
#define PPU_STATUS_SPRITE_0_RESET   0x00

#define PPU_STATUS_MORE_THAN_8_SPRITES_HIT 0x20
#define PPU_STATUS_MORE_THAN_8_SPRITES_RESET 0x00

#endif // Emulator_defines_h

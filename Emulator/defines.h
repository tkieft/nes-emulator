//
//  Header.h
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#ifndef Emulator_Header_h
#define Emulator_Header_h

#include <stdint.h>

#define VRAM_SIZE 0x4000
#define PATTERN_TABLE_SIZE 0x2000
#define SPR_RAM_SIZE 0x100

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
#define VBLANK_INTERRUPT_ENABLE_MASK 0x80

#define VERTICAL_WRITE_ON 0x04
#define VERTICAL_WRITE_OFF 0x00

#define VBLANK_INTERRUPT_ENABLE 0x80
#define VBLANK_INTERRUPT_DISABLE 0x00

/*** PPU CONTROL REGISTER 2 ***/
#define SCREEN_ENABLE_MASK 0x08

#define SCREEN_ENABLE 0x08
#define SCREEN_DISABLE 0x00

/*** PPU STATUS ***/
#define PPU_STATUS_VBLANK_MASK 0x80

#endif
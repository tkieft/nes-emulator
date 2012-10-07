//
//  nes_palette.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef Emulator_nes_palette_h
#define Emulator_nes_palette_h

#include <stdint.h>

typedef struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

extern const color_t NES_PALETTE[64];

#endif

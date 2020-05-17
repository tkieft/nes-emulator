//
//  nes_palette.h
//  Emulator
//
//  Created by Tyler Kieft on 10/6/12.
//
//

#ifndef Emulator_nes_palette_h
#define Emulator_nes_palette_h

#include "defines.h"

typedef struct color {
    byte r;
    byte g;
    byte b;
} color_t;

extern const color_t NES_PALETTE[64];

#endif

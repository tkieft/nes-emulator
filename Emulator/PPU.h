//
//  PPU.h
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#ifndef __Emulator__PPU__
#define __Emulator__PPU__

// PPU CONTROL 1


#define VBLANK_BIT  7
#define VBLANK_MASK 0x80

#include <iostream>

class PPU {
private:
    uint8_t vram[0x4000];
    uint8_t spr_ram[0x100];
    
    uint8_t control_1;
    uint8_t control_2;
    uint8_t status;
    uint8_t sprite_memory_address;
    
    uint8_t read_memory(uint16_t address);
    void store_memory(uint16_t address, uint8_t word);
    
    
public:
    PPU();
    uint8_t read_status();
    
    uint8_t read_control_1();
    void write_control_1(uint8_t value);
    uint8_t read_control_2();
    void write_control_2(uint8_t value);
    
    void write_spr_ram(char* start); // DMA
    
    void set_sprite_memory_address(uint8_t value);
    void set_sprite_data(uint8_t value);
    uint8_t read_sprite_data();
};

#endif /* defined(__Emulator__PPU__) */

//
//  PPU.h
//  Emulator
//
//  Created by Tyler Kieft on 9/26/12.
//
//

#ifndef __Emulator__PPU__
#define __Emulator__PPU__

#include "defines.h"

class PPU {
private:
    uint8_t vram[0x3F20];
    uint8_t spr_ram[0x100];
    
    uint8_t control_1;
    uint8_t control_2;
    uint8_t status;
    uint8_t sprite_memory_address;
    uint16_t vram_address;
    uint8_t horizontal_scroll;
    uint8_t vertical_scroll;
    
    bool first_write;
    
    uint8_t read_memory(uint16_t address);
    void store_memory(uint16_t address, uint8_t word);
    
public:
    PPU();
    void set_chr_rom(uint8_t * chr_rom);

    uint8_t read_status();
    
    uint8_t read_control_1();
    void write_control_1(uint8_t value);
    uint8_t read_control_2();
    void write_control_2(uint8_t value);
    
    void write_spr_ram(char* start); // DMA
    
    void set_sprite_memory_address(uint8_t value);
    void set_sprite_data(uint8_t value);
    uint8_t read_sprite_data();
    
    uint8_t write_scroll_register(uint8_t value);
    
};

#endif /* defined(__Emulator__PPU__) */

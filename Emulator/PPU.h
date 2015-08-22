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
#include "SDLRenderer.h"

class PPU {
    friend class SDLRenderer;
    
private:
    SDLRenderer *renderer;
    
    uint8_t vram[VRAM_SIZE];
    uint8_t spr_ram[SPR_RAM_SIZE];
    
    uint8_t control_1;
    uint8_t control_2;
    uint8_t status;
    uint8_t sprite_memory_address;

    uint8_t read_buffer;
    
    // PPU Scroll Registers
    uint8_t regFV; // Fine vertical scroll latch (3 bits)
    uint8_t regV;  // Vertical name table selection latch (1 bit)
    uint8_t regH;  // Horizontal name table selection latch (1 bit)
    uint8_t regVT; // Vertical tile index latch (5 bits)
    uint8_t regHT; // Horizontal tile index latch (5 bits)
    uint8_t regFH; // Fine horizontal scroll latch (3 bits)
    uint8_t regS;  // Playfield pattern selection table latch
    
    // PPU Scroll counters
    uint8_t cntFV;
    uint8_t cntV;
    uint8_t cntH;
    uint8_t cntVT;
    uint8_t cntHT;
    
    bool first_write;
    
    uint16_t calculate_effective_address(uint16_t address);
    uint8_t read_memory(uint16_t address);
    void store_memory(uint16_t address, uint8_t word);
    
    void reset_vblank_flag();
    
    void reset_sprite_0_flag();
    void set_sprite_0_flag();
    
    void reset_more_than_8_sprites_flag();
    void set_more_than_8_sprites_flag();
    
    uint16_t vram_address();
    uint16_t nametable_address();
    uint16_t attributetable_address();
    uint16_t patterntable_address();
    
    uint8_t palette_select_bits();
    
    void increment_scroll_counters();
    void increment_horizontal_scroll_counter();
    void increment_vertical_scroll_counter();
    void update_scroll_counters_from_registers();
    
    bool is_screen_enabled();
        
public:
    PPU();
    ~PPU();
    
    bool render_scanline(int scanline);
    void set_chr_rom(uint8_t *chr_rom);

    uint8_t read_status();
    uint8_t read_control_1();
    void write_control_1(uint8_t value);
    uint8_t read_control_2();
    void write_control_2(uint8_t value);
    
    void write_spr_ram(char* start); // DMA
    void set_sprite_memory_address(uint8_t value);
    uint8_t read_sprite_data();
    void write_sprite_data(uint8_t value);
        
    void write_scroll_register(uint8_t value);
    
    void write_vram_address(uint8_t value);
    uint8_t read_vram_data();
    void write_vram_data(uint8_t value);
    
};

#endif /* defined(__Emulator__PPU__) */

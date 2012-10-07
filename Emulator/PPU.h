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
#include "glUtil.h"
#include "Renderer.h"

class PPU {
    friend class TerminalRenderer;
    friend class OpenGLRenderer;
    
private:
    Renderer *renderer;
    
    uint8_t vram[VRAM_SIZE];
    uint8_t spr_ram[SPR_RAM_SIZE];
    
    uint8_t control_1;
    uint8_t control_2;
    uint8_t status;
    uint8_t sprite_memory_address;
    uint16_t vram_address;
    uint8_t horizontal_scroll;
    uint8_t vertical_scroll;
    
    bool first_write;
    bool first_read;
    
    uint16_t calculate_effective_address(uint16_t address);
    uint8_t read_memory(uint16_t address);
    void store_memory(uint16_t address, uint8_t word);
    
    void reset_vblank_flag();
        
public:
    PPU();
    ~PPU();
    
    bool render();
    void resize(GLuint width, GLuint height);
    
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

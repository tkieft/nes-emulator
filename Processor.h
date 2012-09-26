//
//  Processor.h
//  Emulator
//
//  Created by Tyler Kieft on 9/24/12.
//
//

#ifndef __Emulator__Processor__
#define __Emulator__Processor__

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

#include <iostream>
#include "stdint.h"

class Processor {
private:
    /* REGISTERS */
    uint16_t pc;     // program counter, 16 bits
    uint8_t s;       // stack pointer
    uint8_t p;       // processor status
    uint8_t a;       // accumulator
    uint8_t x;       // index register x
    uint8_t y;       // index register y
    
    /* MEMORY */
    char *prgRom;
    char *chrRom;
    char *cpuRam;
    char *sram;
    
    uint8_t read_memory(uint16_t address);
    void store_memory(uint16_t address, uint8_t word);
    uint16_t address_at(uint16_t memloc);
    
    /* STACK */
    void stack_push(uint8_t value);
    uint8_t stack_pop();
    
    /* STATUS REGISTER */
    void set_p_bit(uint bit, bool value);
    
    void set_carry(uint8_t result);
    bool if_carry();

    void set_zero(uint8_t result);
    bool if_zero();
    
    void set_interrupt(uint8_t result);
    bool if_interrupt();
    
    void set_decimal(uint8_t value);
    bool if_decimal();
    
    void set_break(uint8_t result);
    bool if_break();
    
    void set_overflow(uint8_t result);
    bool if_overflow();
    
    void set_sign(uint8_t result);
    bool if_sign();
    
    /* HELPER FUNCTIONS */
    uint16_t rel_addr(uint16_t addr, uint8_t offset);
    
public:
    Processor(char *prgRom, char *chrRom);
    void execute();
    void reset();
};

#endif /* defined(__Emulator__Processor__) */

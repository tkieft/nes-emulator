//
//  Processor.h
//  Emulator
//
//  Created by Tyler Kieft on 9/24/12.
//
//

#ifndef __Emulator__Processor__
#define __Emulator__Processor__

#include <iostream>
#include "stdint.h"

#include "defines.h"
#include "PPU.h"
#include "ControllerPad.h"

class Processor {
private:
    PPU *ppu;
    ControllerPad *controller_pad;

    /* REGISTERS */
    uint16_t pc;     // program counter, 16 bits
    uint8_t s;       // stack pointer
    uint8_t p;       // processor status
    uint8_t a;       // accumulator
    uint8_t x;       // index register x
    uint8_t y;       // index register y

    /* MEMORY */
    uint8_t *prg_rom;
    uint8_t *cpu_ram;
    uint8_t *sram;

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
    Processor(PPU *ppu, ControllerPad *controller_pad);
    ~Processor();
    void set_prg_rom(uint8_t *prg_rom);
    void execute();
    void reset();
    void non_maskable_interrupt();
};

#endif /* defined(__Emulator__Processor__) */

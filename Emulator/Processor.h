//
//  Processor.h
//  Emulator
//
//  Created by Tyler Kieft on 9/24/12.
//
//

#ifndef __Emulator__Processor__
#define __Emulator__Processor__

#include <memory>

#include "defines.h"
#include "PPU.h"
#include "ControllerPad.h"

class Processor {
private:
    PPU *ppu;
    ControllerPad *controller_pad;

    /* REGISTERS */
    dbyte pc;     // program counter, 16 bits
    byte s;       // stack pointer
    byte p;       // processor status
    byte a;       // accumulator
    byte x;       // index register x
    byte y;       // index register y

    /* MEMORY */
    static const int kCPURAMSize = 2048;
    static const int kSRAMSize = 8192;

    std::unique_ptr<byte[]> prg_rom;
    byte cpu_ram[kCPURAMSize];
    byte sram[kSRAMSize];

    byte read_memory(dbyte address);
    void store_memory(dbyte address, byte word);
    dbyte address_at(dbyte memloc);

    /* STACK */
    void stack_push(byte value);
    byte stack_pop();

    /* STATUS REGISTER */
    void set_p_bit(int bit, bool value);

    void set_carry(byte result);
    bool if_carry();

    void set_zero(byte result);
    bool if_zero();

    void set_interrupt(byte result);
    bool if_interrupt();

    void set_decimal(byte value);
    bool if_decimal();

    void set_break(byte result);
    bool if_break();

    void set_overflow(byte result);
    bool if_overflow();

    void set_sign(byte result);
    bool if_sign();

    /* HELPER FUNCTIONS */
    dbyte rel_addr(dbyte addr, byte offset);

public:
    Processor(PPU *ppu, ControllerPad *controller_pad);
    void set_prg_rom(std::unique_ptr<byte[]> prg_rom);
    void execute();
    void reset();
    void non_maskable_interrupt();
};

#endif /* defined(__Emulator__Processor__) */

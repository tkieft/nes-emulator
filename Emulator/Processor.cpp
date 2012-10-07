//
//  Processor.cpp
//
//  Emulates the 6502 processor found in the NES.
//  This version of the 6502 does not have BCD (binary-coded decimal) support.
//
//  Created by Tyler Kieft on 9/24/12.
//
//

#include "Processor.h"

Processor::Processor(PPU *ppu) {
    this->ppu = ppu;
    
    prg_rom = NULL;
    cpu_ram = new uint8_t[CPU_RAM_SIZE];
    sram = new uint8_t[SRAM_SIZE];
    
    for (int i = 0; i < CPU_RAM_SIZE; i++) {
        cpu_ram[i] = 0;
    }
    
    for (int i = 0; i < SRAM_SIZE; i++) {
        sram[i] = 0;
    }
}

Processor::~Processor() {
    delete cpu_ram;
    delete sram;
    
    if (prg_rom != NULL) {
        delete prg_rom;
    }
}

void Processor::set_prg_rom(uint8_t *prg_rom) {
    this->prg_rom = prg_rom;
}

void Processor::reset() {
    pc = address_at(0xFFFC);    // address to jump to after reset

    s = 0xFF;
    a = 0;
    p = 1 << UNUSED_BIT;
    x = 0;
    y = 0;
}

void Processor::non_maskable_interrupt() {
    if (if_interrupt()) {
        stack_push(pc >> 8);
        stack_push(pc);
        stack_push(p);
        
        pc = address_at(0xFFFA);
    }
}

void Processor::set_p_bit(uint bit, bool value) {
    value ? p |= 1 << bit : p &= ~(1 << bit);
}

/**
 * If result is nonnegative, set the carry bit. Otherwise, clear it.
 * This means that you should pass in only the higher-order byte(s) of any
 * calculation.
 */
void Processor::set_carry(uint8_t result) {
    set_p_bit(CARRY_BIT, result);
}
bool Processor::if_carry() {
    return p & CARRY_MASK;
}

/**
 * If result is zero, set the zero bit. Otherwise, clear it.
 */
void Processor::set_zero(uint8_t result) {
    set_p_bit(ZERO_BIT, !result);
}
bool Processor::if_zero() {
    return p & ZERO_MASK;
}

void Processor::set_interrupt(uint8_t result) {
    set_p_bit(INTERRUPT_BIT, result);
}
bool Processor::if_interrupt() {
    return p & INTERRUPT_MASK;
}

void Processor::set_decimal(uint8_t result) {
    set_p_bit(DECIMAL_BIT, result);
}
bool Processor::if_decimal() {
    return p & DECIMAL_MASK;
}

void Processor::set_break(uint8_t result) {
    set_p_bit(BREAK_BIT, result);
}
bool Processor::if_break() {
    return p & BREAK_MASK;
}

/**
 * If result is nonzero, set the overflow bit. Otherwise, clear it.
 */
void Processor::set_overflow(uint8_t result) {
    set_p_bit(OVERFLOW_BIT, result);
}
bool Processor::if_overflow() {
    return p & OVERFLOW_MASK;
}

void Processor::set_sign(uint8_t result) {
    // we look at the 7th bit to determine the sign (this happens to be the same
    // bit as the sign bit in the control register)
    set_p_bit(SIGN_BIT, result & SIGN_MASK);
}
bool Processor::if_sign() {
    return p & SIGN_MASK;
}

uint16_t Processor::address_at(uint16_t memloc) {
    return ((uint16_t)read_memory(memloc + 1) << 8) + read_memory(memloc);
}

uint16_t Processor::rel_addr(uint16_t addr, uint8_t offset) {
    // offset is actually a signed number. is there a better way to do this?
    if (offset & SIGN_MASK) {
        return addr - (uint8_t)(~offset + 1);
    } else {
        return addr + offset;
    }
}

void Processor::stack_push(uint8_t value) {
    store_memory(0x100 + s--, value);
}
uint8_t Processor::stack_pop() {
    return read_memory(0x100 + ++s);
}

uint8_t Processor::read_memory(uint16_t address) {
    if (address >= 0x8000) {
        // Program ROM space
        return prg_rom[address - 0x8000];
    } else if (address >= 0x6000) {
        // SRAM
        return sram[address - 0x6000];
    } else if (address >= 0x4020) {
        // Expansion ROM
        //throw "Expansion ROM not implemented";
    } else if (address >= 0x4000) {
        // Other I/O registers
        return 0;
    } else if (address >= 0x2000) {
        // PPU I/O Registers
        switch (address & 0x07) {           // I/O registers are mirrored every 8 bytes
            case 0x00:
                return ppu->read_control_1();
            case 0x01:
                return ppu->read_control_2();
            case 0x02:
                return ppu->read_status();
            case 0x04:
                return ppu->read_sprite_data();
            case 0x07:
                return ppu->read_vram_data();
            default:
                throw "Unrecognized I/O read.";
        }
    }
    
    // Must be in 0x0000 -> 0x2000 (CPU RAM, mirrored 4x)
    return cpu_ram[address & 0x07FF];
}

void Processor::store_memory(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        // CPU Ram
        cpu_ram[address & 0x07FF] = value;
    } else if (address < 0x4000) {
        switch (address & 0x07) {
            case 0x00:
                ppu->write_control_1(value);
                break;
            case 0x01:
                ppu->write_control_2(value);
                break;
            case 0x03:
                ppu->set_sprite_memory_address(value);
                break;
            case 0x04:
                ppu->write_sprite_data(value);
                break;
            case 0x05:
                ppu->write_scroll_register(value);
                break;
            case 0x06:
                ppu->write_vram_address(value);
                break;
            case 0x07:
                ppu->write_vram_data(value);
                break;
            default:
                throw "Unrecognized I/O write. Please implement!";
        }
    } else if (address < 0x4020) {
        // Sound and other I/O registers
        switch (address) {
            case 0x4014:
                ppu->write_spr_ram((char *)(cpu_ram + value * 0x100));
                break;
            default:
                break;
                //throw "Unrecognized I/O write. Please implement.";
        }
    } else if (address >= 0x6000 && address < 0x8000) {
        // SRAM
        sram[address - 0x6000] = value;
    }
}

void Processor::execute() {
    uint8_t opcode = read_memory(pc);   // opcode of instruction
    uint16_t address;                   // address of operand
    uint8_t src;                        // operand
    uint16_t temp;                      // larger temp var for calculations
    
    /*** ADDRESSING MODE ***/
    switch (opcode) {
        // Immediate
        case 0x69: case 0x29: case 0xC9: case 0xE0: case 0xC0: case 0x49: case 0xA9:
        case 0xA2: case 0xA0: case 0x09: case 0xE9:
            address = pc + 1;
            pc += 2;
            break;
        // Zero-page
        case 0x65: case 0x25: case 0x06: case 0x24: case 0xC5: case 0xE4: case 0xC4:
        case 0xC6: case 0x45: case 0xE6: case 0xA5: case 0xA6: case 0xA4: case 0x46:
        case 0x05: case 0x26: case 0x66: case 0xE5: case 0x85: case 0x86: case 0x84:
            address = read_memory(pc + 1);
            pc += 2;
            break;
        // Zero-page,X-indexed
        case 0x75: case 0x35: case 0x16: case 0xD5: case 0xD6: case 0x55: case 0xF6:
        case 0xB5: case 0xB6: case 0xB4: case 0x56: case 0x15: case 0x36: case 0x76:
        case 0xF5: case 0x95: case 0x94:
            address = (read_memory(pc + 1) + x) & 0xFF; // Wrap-around addition
            pc += 2;
            break;
        // Zero-page,Y-indexed
        case 0x96:
            address = (read_memory(pc + 1) + y) & 0xFF; // Wrap-around addition
            pc += 2;
            break;
        // Implied
        case 0x00: case 0x18: case 0xD8: case 0x58: case 0xB8: case 0xCA: case 0x88:
        case 0xE8: case 0xC8: case 0xEA: case 0x48: case 0x08: case 0x68: case 0x28:
        case 0x40: case 0x60: case 0x38: case 0xF8: case 0x78: case 0xAA: case 0xA8:
        case 0xBA: case 0x8A: case 0x9A: case 0x98:
            pc += 1;
            break;
        // Accumulator:
        case 0x0A: case 0x4A: case 0x2A: case 0x6A:
            src = a;
            pc += 1;
            break;
        // Absolute
        case 0x6D: case 0x2D: case 0x0E: case 0x2C: case 0xCD: case 0xEC: case 0xCC:
        case 0xCE: case 0x4D: case 0xEE: case 0x4C: case 0x20: case 0xAD: case 0xAE:
        case 0xAC: case 0x4E: case 0x0D: case 0x2E: case 0x6E: case 0xED: case 0x8D:
        case 0x8E: case 0x8C:
            address = address_at(pc + 1);
            pc += 3;
            break;
        // Absolute,X-indexed
        case 0x7D: case 0x3D: case 0x1E: case 0xDD: case 0xDE: case 0x5D: case 0xFE:
        case 0xBD: case 0xBC: case 0x5E: case 0x1D: case 0x3E: case 0x7E: case 0xFD:
        case 0x9D:
            address = address_at(pc + 1) + x;
            pc += 3;
            break;
        // Absolute,Y-indexed
        case 0x79: case 0x39: case 0xD9: case 0x59: case 0xB9: case 0xBE: case 0x19:
        case 0xF9: case 0x99:
            address = address_at(pc + 1) + y;
            pc += 3;
            break;
        // Indirect
        case 0x6C:
            address = address_at(address_at(pc + 1));
            pc += 3;
            break;
        // (Indirect,pre-X-indexed)
        case 0x61: case 0x21: case 0xC1: case 0x41: case 0xA1: case 0x01: case 0xE1:
        case 0x81:
            address = address_at((read_memory(pc + 1) + x) & 0xFF); // Wrap-around addition
            pc += 2;
            break;
        // (Indirect),Y-post-indexed
        case 0x71: case 0x31: case 0xD1: case 0x51: case 0xB1: case 0x11: case 0xF1:
        case 0x91:
            address = address_at(read_memory(pc + 1)) + y;
            pc += 2;
            break;
        // Relative
        case 0x90: case 0xB0: case 0xF0: case 0x30: case 0xD0: case 0x10: case 0x50:
        case 0x70:
            address = pc + 1;
            pc += 2;
            break;
            
        default:
            std::cerr << "[Addressing mode] "
                      << "Unrecognized opcode: " << std::hex << opcode
                      << "at PC: " << std::hex << pc;
    }
    
    switch (opcode) {
        /* ADC (Add w/ carry) */
        case 0x69: case 0x65: case 0x75: case 0x6D: case 0x7D: case 0x79: case 0x61: case 0x71:
            src = read_memory(address);
            temp = (uint16_t)a + src + (if_carry() ? 1 : 0);

            set_carry(temp > 0xFF);
            set_zero(temp & 0xFF); // only look at last byte
            set_sign(temp);
            // If we add two numbers of the same sign and get a different sign, there's overflow
            set_overflow(!((a ^ src) & SIGN_MASK) && ((a ^ temp) & SIGN_MASK));

            a = (uint8_t)temp;
            break;

            
        /* AND */
        case 0x29: case 0x25: case 0x35: case 0x2D: case 0x3D: case 0x39: case 0x21: case 0x31:
            a &= read_memory(address);
            set_zero(a);
            set_sign(a);
            break;
        
        /* ASL (Shift left one bit) */
        case 0x0A: case 0x06: case 0x16: case 0x0E: case 0x1E:
            if (opcode != 0x0A) src = read_memory(address);
            
            set_carry(src & 0x80);
            src <<= 1;
            set_zero(src);
            set_sign(src);
            
            if (opcode == 0x0A)
                a = src;
            else
                store_memory(address, src);
            
            break;
        
        /* BCC (Branch on carry clear) (C = 0) */
        case 0x90:
            src = read_memory(address);
            if (!if_carry()) {
                pc = rel_addr(pc, src);
            }
            break;
        
        /* BCS (Branch on carry set) (C = 1) */
        case 0xB0:
            src = read_memory(address);
            if (if_carry()) {
                pc = rel_addr(pc, src);
            }
            break;
            
        /* BEQ (Branch on equal) (Z = 1) */
        case 0xF0:
            src = read_memory(address);
            if (if_zero()) {
                pc = rel_addr(pc, src);
            }
            break;
        
        /* BIT (Test bits in memory with accumulator) */
        case 0x24: case 0x2C:
            src = read_memory(address);
            set_overflow(src & OVERFLOW_MASK);
            set_sign(src);
            set_zero(a & src);
            break;
        
        /* BMI (Branch on result minus) (N = 1) */
        case 0x30:
            src = read_memory(address);
            if (if_sign()) {
                pc = rel_addr(pc, src);
            }
            break;
            
        /* BNE (Branch on not equal) (Z = 0) */
        case 0xD0:
            src = read_memory(address);
            if (!if_zero()) {
                pc = rel_addr(pc, src);
            }
            break;
        
        /* BPL (Branch on result plus) (N = 0) */
        case 0x10:
            src = read_memory(address);
            if (!if_sign()) {
                pc = rel_addr(pc, src);
            }
            break;
            
        /* BRK (Force break) */
        case 0x00:
            stack_push(pc >> 8);
            stack_push(pc);
            set_break(1);
            stack_push(p);
            set_interrupt(1); // disable interrupts
            pc = address_at(0xFFFE);
            break;
        
        /* BVC (Branch on V clear) (V = 0)*/
        case 0x50:
            src = read_memory(address);
            if (!if_overflow()) {
                pc = rel_addr(pc, src);
            }
            break;
        
        /* CLC (Clear carry flag) */
        case 0x18:
            set_carry(0);
            break;
        
        /* CLD (Clear decimal mode) */
        case 0xD8:
            set_decimal(0);
            break;
        
        /* CLI (Clear interrupt disable bit) */
        case 0x58:
            set_interrupt(0);
            break;
        
        /* CLV (Clear overflow flag) */
        case 0xB8:
            set_overflow(0);
            break;
        
        /* CMP (Compare memory and accumulator) */
        case 0xC9: case 0xC5: case 0xD5: case 0xCD: case 0xDD: case 0xD9: case 0xC1: case 0xD1:
            temp = (uint16_t)a - read_memory(address);
            set_sign(temp);
            set_zero(temp);
            set_carry(temp < 0x100); // if a > src, carry set
            break;
            
        /* CPX (Compare memory and index x) */
        case 0xE0: case 0xE4: case 0xEC:
            temp = (uint16_t)x - read_memory(address);
            set_sign(temp);
            set_zero(temp);
            set_carry(temp < 0x100); // if x > src, carry set
            break;
            
        /* CPY (Compare memory and index y) */
        case 0xC0: case 0xC4: case 0xCC:
            temp = (uint16_t)y - read_memory(address);
            set_sign(temp);
            set_zero(temp);
            set_carry(temp < 0x100); // if y > src, carry set
            break;
        
        /* DEC (Decrement memory by 1) */
        case 0xC6: case 0xD6: case 0xCE: case 0xDE:
            src = read_memory(address) - 1;
            set_zero(src);
            set_sign(src);
            store_memory(address, src);
            break;
        
        /* DEX (Decrement index X by 1) */
        case 0xCA:
            x--;
            set_zero(x);
            set_sign(x);
            break;

        /* DEY (Decrement index Y by 1) */
        case 0x88:
            y--;
            set_zero(y);
            set_sign(y);
            break;
            
        /* EOR (XOR memory with accumulator) */
        case 0x49: case 0x45: case 0x55: case 0x4D: case 0x5D: case 0x59: case 0x41: case 0x51:
            a ^= read_memory(address);
            set_sign(a);
            set_zero(a);
            break;
        
        /* INC (Increment memory by one) */
        case 0xE6: case 0xF6: case 0xEE: case 0xFE:
            src = read_memory(address) + 1;
            set_sign(src);
            set_zero(src);
            store_memory(address, src);
            break;
        
        /* INX (Increment index X by one) */
        case 0xE8:
            x++;
            set_sign(x);
            set_zero(x);
            break;
        
        /* INY (Increment index Y by one) */
        case 0xC8:
            y++;
            set_sign(y);
            set_zero(y);
            break;
        
        /* JMP (Jump to new location) */
        case 0x4C: case 0x6C:
            pc = address;
            break;
        
        /* JSR (Jump to new location saving return address */
        case 0x20:
            pc--;
            stack_push(pc >> 8);        // Push the higher byte first
            stack_push(pc);
            pc = address;
            break;
        
        /* LDA (Load accumulator with memory) */
        case 0xA9: case 0xA5: case 0xB5: case 0xAD: case 0xBD: case 0xB9: case 0xA1: case 0xB1:
            a = read_memory(address);
            set_sign(a);
            set_zero(a);
            break;
        
        /* LDX (Load index X with memory) */
        case 0xA2: case 0xA6: case 0xB6: case 0xAE: case 0xBE:
            x = read_memory(address);
            set_sign(x);
            set_zero(x);
            break;
        
        /* LDY (Load index Y with memory) */
        case 0xA0: case 0xA4: case 0xB4: case 0xAC: case 0xBC:
            y = read_memory(address);
            set_sign(y);
            set_zero(y);
            break;
        
        /* LSR (Shift right one bit) */
        case 0x4A: case 0x46: case 0x56: case 0x4E: case 0x5E:
            if (opcode != 0x4A) src = read_memory(address);
            
            set_sign(0);
            set_carry(src & 0x01);
            src >>= 1;
            set_zero(src);
            
            if (opcode == 0x4A) {
                a = src;
            } else {
                store_memory(address, src);
            }
            break;
        
        /* NOP (no-operation) */
        case 0xEA:
            break;
            
        /* ORA (Or memory with accumulator) */
        case 0x09: case 0x05: case 0x15: case 0x0D: case 0x1D: case 0x19: case 0x01: case 0x11:
            a |= read_memory(address);
            set_sign(a);
            set_zero(a);
            break;
        
        /* PHA (Push accumulator on stack) */
        case 0x48:
            stack_push(a);
            break;
        
        /* PHP (Push processor status on stack) */
        case 0x08:
            stack_push(p);
            break;
            
        /* PLA (Pull accumulator from stack) */
        case 0x68:
            a = stack_pop();
            set_sign(a);
            set_zero(a);
            break;
            
        /* PLP (Pull processor status from stack) */
        case 0x28:
            p = stack_pop();
            break;
            
        /* ROL (Rotate one bit left) (memory or accumulator) */
        case 0x2A: case 0x26: case 0x36: case 0x2E: case 0x3E:
            if (opcode != 0x2A) src = read_memory(address);
            
            temp = (uint16_t)src << 1;
            if (if_carry()) temp |= 0x01;
            set_carry(temp >> 8);
            src = (uint8_t)temp;
            set_sign(src);
            set_zero(src);
            
            if (opcode == 0x2A) {
                a = src;
            } else {
                store_memory(address, src);
            }
            break;
        
        /* ROR (Rotate one bit right) (memory or accumulator) */
        case 0x6A: case 0x66: case 0x76: case 0x6E: case 0x7E:
            if (opcode != 0x6A) src = read_memory(address);
            
            temp = src & 0x01;
            src >>= 1;
            if (if_carry()) src |= 0x80;
            set_carry(temp);
            set_sign(src);
            set_zero(src);
            
            if (opcode == 0x6A) {
                a = src;
            } else {
                store_memory(address, src);
            }
            
            break;
     
        /* RTI (Return from interrupt) */
        case 0x40:
            p = stack_pop();
            // Must make this two instructions so that the compiler doesn't screw us.
            pc = stack_pop();   // Pop the lower byte first
            pc |= (uint16_t)stack_pop() << 8;
            break;
            
        /* RTS (Return from subroutine) */
        case 0x60:
            pc = stack_pop();   // Pop the lower byte first
            pc |= (uint16_t)stack_pop() << 8;
            pc++; // Must add 1
            break;
        
        /* SBC (Subtract with carry) */
        case 0xE9: case 0xE5: case 0xF5: case 0xED: case 0xFD: case 0xF9: case 0xE1: case 0xF1:
            src = read_memory(address);
            temp = (uint16_t)a - src - (if_carry() ? 0 : 1);
            set_zero(temp & 0xFF);
            set_sign(temp);
            set_overflow(((a ^ temp) & 0x80) && ((a ^ src) & 0x80));
            set_carry(temp < 0x100);
            a = (uint8_t)temp;
            break;
            
        /* SEC (Set carry bit) */
        case 0x38:
            set_carry(1);
            break;
        
        /* SED (Set decimal mode) */
        case 0xF8:
            set_decimal(1);
            break;
        
        /* SEI (Set interrupt disable status) */
        case 0x78:
            set_interrupt(1);
            break;
            
        /* STA (Store accumulator in memory) */
        case 0x85: case 0x95: case 0x8D: case 0x9D: case 0x99: case 0x81: case 0x91:
            store_memory(address, a);
            break;
            
        /* STX (Store index X in memory) */
        case 0x86: case 0x96: case 0x8E:
            store_memory(address, x);
            break;
            
        /* STY (Store index Y in memory) */
        case 0x84: case 0x94: case 0x8C:
            store_memory(address, y);
            break;
            
        /* TAX (Transfer accumulator to index X) */
        case 0xAA:
            x = a;
            set_sign(x);
            set_zero(x);
            break;
        
        /* TAY (Transfer accumulator to index Y) */
        case 0xA8:
            y = a;
            set_sign(y);
            set_zero(y);
            break;
            
        /* TSX (Transfer stack pointer to index X) */
        case 0xBA:
            x = s;
            set_sign(x);
            set_zero(x);
            break;
            
        /* TXA (Transfer index X to accumulator) */
        case 0x8A:
            a = x;
            set_sign(a);
            set_zero(a);
            break;
        
        /* TXS (Transfer index X to stack pointer) */
        case 0x9A:
            s = x;
            break;
        
        /* TYA (Transfer index Y to accumulator) */
        case 0x98:
            a = y;
            set_sign(a);
            set_zero(a);
            break;
        
        default:
            std::cerr << "[Execution] "
                      << "Unrecognized opcode: " << std::hex << opcode
                      << "at PC: " << std::hex << pc;
    }
}


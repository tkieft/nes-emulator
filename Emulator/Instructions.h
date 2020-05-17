//
//  Instructions.h
//  Emulator
//
//  Created by Tyler Kieft on 9/11/16.
//
//

#ifndef Instructions_h
#define Instructions_h

#include <cstdint>

enum AddressType {
    Immediate,
    ZeroPage,
    ZeroPageX,     // Zero-page, X-indexed
    ZeroPageY,     // Zero-page, Y-indexed
    Implied,
    Accumulator,
    Absolute,
    AbsoluteX,     // Absolute, X-indexed
    AbsoluteY,     // Absolute, Y-indexed
    Indirect,      // JMP
    IndirectPreX,  // Indirect, pre-X-indexed
    IndirectPostY, // Indirect, post-Y-indexed
    Relative,
};

enum Function {
    ADC,  // (Add w/ carry)
    AND,  // (AND memory with accumulator)
    ASL,  // (Shift left one bit)
    BCC,  // (Branch on carry clear) (C = 0)
    BCS,  // (Branch on carry set) (C = 1)
    BEQ,  // (Branch on equal) (Z = 1)
    BIT,  // (Test bits in memory with accumulator)
    BMI,  // (Branch on result minus) (N = 1)
    BNE,  // (Branch on not equal) (Z = 0)
    BPL,  // (Branch on result plus) (N = 0)
    BRK,  // (Force break)
    BVC,  // (Branch on V clear) (V = 0)
    BVS,  // (Branch on V set) (V = 1)
    CLC,  // (Clear carry flag)
    CLD,  // (Clear decimal mode)
    CLI,  // (Clear interrupt disable bit)
    CLV,  // (Clear overflow flag)
    CMP,  // (Compare memory and accumulator)
    CPX,  // (Compare memory and index x)
    CPY,  // (Compare memory and index y)
    DEC,  // (Decrement memory by 1)
    DEX,  // (Decrement index X by 1)
    DEY,  // (Decrement index Y by 1)
    EOR,  // (XOR memory with accumulator)
    INC,  // (Increment memory by one)
    INX,  // (Increment index X by one)
    INY,  // (Increment index Y by one)
    JMP,  // (Jump to new location)
    JSR,  // (Jump to new location saving return address)
    LDA,  // (Load accumulator with memory)
    LDX,  // (Load index X with memory)
    LDY,  // (Load index Y with memory)
    LSR,  // (Shift right one bit)
    NOP,  // (no-operation)
    ORA,  // (Or memory with accumulator)
    PHA,  // (Push accumulator on stack)
    PHP,  // (Push processor status on stack)
    PLA,  // (Pull accumulator from stack)
    PLP,  // (Pull processor status from stack)
    ROL,  // (Rotate one bit left) (memory or accumulator)
    ROR,  // (Rotate one bit right) (memory or accumulator)
    RTI,  // (Return from interrupt)
    RTS,  // (Return from subroutine)
    SBC,  // (Subtract with carry)
    SEC,  // (Set carry bit)
    SED,  // (Set decimal mode)
    SEI,  // (Set interrupt disable status)
    STA,  // (Store accumulator in memory)
    STX,  // (Store index X in memory)
    STY,  // (Store index Y in memory)
    TAX,  // (Transfer accumulator to index X)
    TAY,  // (Transfer accumulator to index Y)
    TSX,  // (Transfer stack pointer to index X)
    TXA,  // (Transfer index X to accumulator)
    TXS,  // (Transfer index X to stack pointer)
    TYA,  // (Transfer index Y to accumulator)
};

typedef struct Instruction {
    Function function;
    AddressType address_type;
} Instruction;

extern Instruction get_instruction(uint8_t opcode);

#endif /* Instructions_h */

//
//  Instructions.cpp
//  Emulator
//
//  Created by Tyler Kieft on 9/11/16.
//
//

#include "Instructions.h"

#include <map>

#include "defines.h"

// clang-format off
static std::map<byte, Instruction> INSTRUCTIONS = {
    {0x00, {BRK, Implied}},
    {0x01, {ORA, IndirectPreX}},
    {0x05, {ORA, ZeroPage}},
    {0x06, {ASL, ZeroPage}},
    {0x08, {PHP, Implied}},
    {0x09, {ORA, Immediate}},
    {0x0A, {ASL, Accumulator}},
    {0x0D, {ORA, Absolute}},
    {0x0E, {ASL, Absolute}},

    {0x10, {BPL, Relative}},
    {0x11, {ORA, IndirectPostY}},
    {0x15, {ORA, ZeroPageX}},
    {0x16, {ASL, ZeroPageX}},
    {0x18, {CLC, Implied}},
    {0x19, {ORA, AbsoluteY}},
    {0x1D, {ORA, AbsoluteX}},
    {0x1E, {ASL, AbsoluteX}},

    {0x20, {JSR, Absolute}},
    {0x21, {AND, IndirectPreX}},
    {0x24, {BIT, ZeroPage}},
    {0x25, {AND, ZeroPage}},
    {0x26, {ROL, ZeroPage}},
    {0x28, {PLP, Implied}},
    {0x29, {AND, Immediate}},
    {0x2A, {ROL, Accumulator}},
    {0x2C, {BIT, Absolute}},
    {0x2D, {AND, Absolute}},
    {0x2E, {ROL, Absolute}},

    {0x30, {BMI, Relative}},
    {0x31, {AND, IndirectPostY}},
    {0x35, {AND, ZeroPageX}},
    {0x36, {ROL, ZeroPageX}},
    {0x38, {SEC, Implied}},
    {0x39, {AND, AbsoluteY}},
    {0x3D, {AND, AbsoluteX}},
    {0x3E, {ROL, AbsoluteX}},

    {0x40, {RTI, Implied}},
    {0x41, {EOR, IndirectPreX}},
    {0x45, {EOR, ZeroPage}},
    {0x46, {LSR, ZeroPage}},
    {0x48, {PHA, Implied}},
    {0x49, {EOR, Immediate}},
    {0x4A, {LSR, Accumulator}},
    {0x4C, {JMP, Absolute}},
    {0x4D, {EOR, Absolute}},
    {0x4E, {LSR, Absolute}},

    {0x50, {BVC, Relative}},
    {0x51, {EOR, IndirectPostY}},
    {0x55, {EOR, ZeroPageX}},
    {0x56, {LSR, ZeroPageX}},
    {0x58, {CLI, Implied}},
    {0x59, {EOR, AbsoluteY}},
    {0x5D, {EOR, AbsoluteX}},
    {0x5E, {LSR, AbsoluteX}},

    {0x60, {RTS, Implied}},
    {0x61, {ADC, IndirectPreX}},
    {0x65, {ADC, ZeroPage}},
    {0x66, {ROR, ZeroPage}},
    {0x68, {PLA, Implied}},
    {0x69, {ADC, Immediate}},
    {0x6A, {ROR, Accumulator}},
    {0x6C, {JMP, Indirect}},
    {0x6D, {ADC, Absolute}},
    {0x6E, {ROR, Absolute}},

    {0x70, {BVS, Relative}},
    {0x71, {ADC, IndirectPostY}},
    {0x75, {ADC, ZeroPageX}},
    {0x76, {ROR, ZeroPageX}},
    {0x78, {SEI, Implied}},
    {0x79, {ADC, AbsoluteY}},
    {0x7D, {ADC, AbsoluteX}},
    {0x7E, {ROR, AbsoluteX}},

    {0x81, {STA, IndirectPreX}},
    {0x84, {STY, ZeroPage}},
    {0x85, {STA, ZeroPage}},
    {0x86, {STX, ZeroPage}},
    {0x88, {DEY, Implied}},
    {0x8A, {TXA, Implied}},
    {0x8C, {STY, Absolute}},
    {0x8D, {STA, Absolute}},
    {0x8E, {STX, Absolute}},

    {0x90, {BCC, Relative}},
    {0x91, {STA, IndirectPostY}},
    {0x94, {STY, ZeroPageX}},
    {0x95, {STA, ZeroPageX}},
    {0x96, {STX, ZeroPageY}},
    {0x98, {TYA, Implied}},
    {0x99, {STA, AbsoluteY}},
    {0x9A, {TXS, Implied}},
    {0x9D, {STA, AbsoluteX}},

    {0xA0, {LDY, Immediate}},
    {0xA1, {LDA, IndirectPreX}},
    {0xA2, {LDX, Immediate}},
    {0xA4, {LDY, ZeroPage}},
    {0xA5, {LDA, ZeroPage}},
    {0xA6, {LDX, ZeroPage}},
    {0xA8, {TAY, Implied}},
    {0xA9, {LDA, Immediate}},
    {0xAA, {TAX, Implied}},
    {0xAC, {LDY, Absolute}},
    {0xAD, {LDA, Absolute}},
    {0xAE, {LDX, Absolute}},

    {0xB0, {BCS, Relative}},
    {0xB1, {LDA, IndirectPostY}},
    {0xB4, {LDY, ZeroPageX}},
    {0xB5, {LDA, ZeroPageX}},
    {0xB6, {LDX, ZeroPageY}},
    {0xB8, {CLV, Implied}},
    {0xB9, {LDA, AbsoluteY}},
    {0xBA, {TSX, Implied}},
    {0xBC, {LDY, AbsoluteX}},
    {0xBD, {LDA, AbsoluteX}},
    {0xBE, {LDX, AbsoluteY}},

    {0xC0, {CPY, Immediate}},
    {0xC1, {CMP, IndirectPreX}},
    {0xC4, {CPY, ZeroPage}},
    {0xC5, {CMP, ZeroPage}},
    {0xC6, {DEC, ZeroPage}},
    {0xC8, {INY, Implied}},
    {0xC9, {CMP, Immediate}},
    {0xCA, {DEX, Implied}},
    {0xCC, {CPY, Absolute}},
    {0xCD, {CMP, Absolute}},
    {0xCE, {DEC, Absolute}},

    {0xD0, {BNE, Relative}},
    {0xD1, {CMP, IndirectPostY}},
    {0xD5, {CMP, ZeroPageX}},
    {0xD6, {DEC, ZeroPageX}},
    {0xD8, {CLD, Implied}},
    {0xD9, {CMP, AbsoluteY}},
    {0xDD, {CMP, AbsoluteX}},
    {0xDE, {DEC, AbsoluteX}},

    {0xE0, {CPX, Immediate}},
    {0xE1, {SBC, IndirectPreX}},
    {0xE4, {CPX, ZeroPage}},
    {0xE5, {SBC, ZeroPage}},
    {0xE6, {INC, ZeroPage}},
    {0xE8, {INX, Implied}},
    {0xE9, {SBC, Immediate}},
    {0xEA, {NOP, Implied}},
    {0xEC, {CPX, Absolute}},
    {0xED, {SBC, Absolute}},
    {0xEE, {INC, Absolute}},

    {0xF0, {BEQ, Relative}},
    {0xF1, {SBC, IndirectPostY}},
    {0xF5, {SBC, ZeroPageX}},
    {0xF6, {INC, ZeroPageX}},
    {0xF8, {SED, Implied}},
    {0xF9, {SBC, AbsoluteY}},
    {0xFD, {SBC, AbsoluteX}},
    {0xFE, {INC, AbsoluteX}},
};
// clang-format on

Instruction get_instruction(byte opcode) {
  // Throws an exception if opcode not found
  return INSTRUCTIONS.at(opcode);
}

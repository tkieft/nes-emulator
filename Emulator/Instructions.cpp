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
  {0x00, {BRK, Implied,       7}},
  {0x01, {ORA, IndirectPreX,  6}},
  {0x05, {ORA, ZeroPage,      3}},
  {0x06, {ASL, ZeroPage,      5}},
  {0x08, {PHP, Implied,       3}},
  {0x09, {ORA, Immediate,     2}},
  {0x0A, {ASL, Accumulator,   2}},
  {0x0D, {ORA, Absolute,      4}},
  {0x0E, {ASL, Absolute,      6}},

  {0x10, {BPL, Relative,      3}},
  {0x11, {ORA, IndirectPostY, 5}},
  {0x15, {ORA, ZeroPageX,     4}},
  {0x16, {ASL, ZeroPageX,     6}},
  {0x18, {CLC, Implied,       2}},
  {0x19, {ORA, AbsoluteY,     4}},
  {0x1D, {ORA, AbsoluteX,     4}},
  {0x1E, {ASL, AbsoluteX,     7}},

  {0x20, {JSR, Absolute,      6}},
  {0x21, {AND, IndirectPreX,  6}},
  {0x24, {BIT, ZeroPage,      3}},
  {0x25, {AND, ZeroPage,      3}},
  {0x26, {ROL, ZeroPage,      5}},
  {0x28, {PLP, Implied,       4}},
  {0x29, {AND, Immediate,     2}},
  {0x2A, {ROL, Accumulator,   2}},
  {0x2C, {BIT, Absolute,      4}},
  {0x2D, {AND, Absolute,      4}},
  {0x2E, {ROL, Absolute,      6}},

  {0x30, {BMI, Relative,      3}},
  {0x31, {AND, IndirectPostY, 5}},
  {0x35, {AND, ZeroPageX,     4}},
  {0x36, {ROL, ZeroPageX,     6}},
  {0x38, {SEC, Implied,       2}},
  {0x39, {AND, AbsoluteY,     4}},
  {0x3D, {AND, AbsoluteX,     4}},
  {0x3E, {ROL, AbsoluteX,     7}},

  {0x40, {RTI, Implied,       6}},
  {0x41, {EOR, IndirectPreX,  6}},
  {0x45, {EOR, ZeroPage,      3}},
  {0x46, {LSR, ZeroPage,      5}},
  {0x48, {PHA, Implied,       3}},
  {0x49, {EOR, Immediate,     2}},
  {0x4A, {LSR, Accumulator,   2}},
  {0x4C, {JMP, Absolute,      3}},
  {0x4D, {EOR, Absolute,      4}},
  {0x4E, {LSR, Absolute,      6}},

  {0x50, {BVC, Relative,      3}},
  {0x51, {EOR, IndirectPostY, 5}},
  {0x55, {EOR, ZeroPageX,     4}},
  {0x56, {LSR, ZeroPageX,     6}},
  {0x58, {CLI, Implied,       2}},
  {0x59, {EOR, AbsoluteY,     4}},
  {0x5D, {EOR, AbsoluteX,     4}},
  {0x5E, {LSR, AbsoluteX,     7}},

  {0x60, {RTS, Implied,       6}},
  {0x61, {ADC, IndirectPreX,  6}},
  {0x65, {ADC, ZeroPage,      3}},
  {0x66, {ROR, ZeroPage,      5}},
  {0x68, {PLA, Implied,       4}},
  {0x69, {ADC, Immediate,     2}},
  {0x6A, {ROR, Accumulator,   2}},
  {0x6C, {JMP, Indirect,      5}},
  {0x6D, {ADC, Absolute,      4}},
  {0x6E, {ROR, Absolute,      6}},

  {0x70, {BVS, Relative,      3}},
  {0x71, {ADC, IndirectPostY, 5}},
  {0x75, {ADC, ZeroPageX,     4}},
  {0x76, {ROR, ZeroPageX,     6}},
  {0x78, {SEI, Implied,       2}},
  {0x79, {ADC, AbsoluteY,     4}},
  {0x7D, {ADC, AbsoluteX,     4}},
  {0x7E, {ROR, AbsoluteX,     7}},

  {0x81, {STA, IndirectPreX,  6}},
  {0x84, {STY, ZeroPage,      3}},
  {0x85, {STA, ZeroPage,      3}},
  {0x86, {STX, ZeroPage,      3}},
  {0x88, {DEY, Implied,       2}},
  {0x8A, {TXA, Implied,       2}},
  {0x8C, {STY, Absolute,      4}},
  {0x8D, {STA, Absolute,      4}},
  {0x8E, {STX, Absolute,      4}},

  {0x90, {BCC, Relative,      3}},
  {0x91, {STA, IndirectPostY, 6}},
  {0x94, {STY, ZeroPageX,     4}},
  {0x95, {STA, ZeroPageX,     4}},
  {0x96, {STX, ZeroPageY,     4}},
  {0x98, {TYA, Implied,       2}},
  {0x99, {STA, AbsoluteY,     5}},
  {0x9A, {TXS, Implied,       2}},
  {0x9D, {STA, AbsoluteX,     5}},

  {0xA0, {LDY, Immediate,     2}},
  {0xA1, {LDA, IndirectPreX,  6}},
  {0xA2, {LDX, Immediate,     2}},
  {0xA4, {LDY, ZeroPage,      3}},
  {0xA5, {LDA, ZeroPage,      3}},
  {0xA6, {LDX, ZeroPage,      3}},
  {0xA8, {TAY, Implied,       2}},
  {0xA9, {LDA, Immediate,     2}},
  {0xAA, {TAX, Implied,       2}},
  {0xAC, {LDY, Absolute,      4}},
  {0xAD, {LDA, Absolute,      4}},
  {0xAE, {LDX, Absolute,      4}},

  {0xB0, {BCS, Relative,      3}},
  {0xB1, {LDA, IndirectPostY, 5}},
  {0xB4, {LDY, ZeroPageX,     4}},
  {0xB5, {LDA, ZeroPageX,     4}},
  {0xB6, {LDX, ZeroPageY,     4}},
  {0xB8, {CLV, Implied,       2}},
  {0xB9, {LDA, AbsoluteY,     4}},
  {0xBA, {TSX, Implied,       2}},
  {0xBC, {LDY, AbsoluteX,     4}},
  {0xBD, {LDA, AbsoluteX,     4}},
  {0xBE, {LDX, AbsoluteY,     4}},

  {0xC0, {CPY, Immediate,     2}},
  {0xC1, {CMP, IndirectPreX,  6}},
  {0xC4, {CPY, ZeroPage,      3}},
  {0xC5, {CMP, ZeroPage,      3}},
  {0xC6, {DEC, ZeroPage,      5}},
  {0xC8, {INY, Implied,       2}},
  {0xC9, {CMP, Immediate,     2}},
  {0xCA, {DEX, Implied,       2}},
  {0xCC, {CPY, Absolute,      4}},
  {0xCD, {CMP, Absolute,      4}},
  {0xCE, {DEC, Absolute,      6}},

  {0xD0, {BNE, Relative,      3}},
  {0xD1, {CMP, IndirectPostY, 5}},
  {0xD5, {CMP, ZeroPageX,     4}},
  {0xD6, {DEC, ZeroPageX,     6}},
  {0xD8, {CLD, Implied,       2}},
  {0xD9, {CMP, AbsoluteY,     4}},
  {0xDD, {CMP, AbsoluteX,     4}},
  {0xDE, {DEC, AbsoluteX,     7}},

  {0xE0, {CPX, Immediate,     2}},
  {0xE1, {SBC, IndirectPreX,  6}},
  {0xE4, {CPX, ZeroPage,      3}},
  {0xE5, {SBC, ZeroPage,      3}},
  {0xE6, {INC, ZeroPage,      5}},
  {0xE8, {INX, Implied,       2}},
  {0xE9, {SBC, Immediate,     2}},
  {0xEA, {NOP, Implied,       2}},
  {0xEC, {CPX, Absolute,      4}},
  {0xED, {SBC, Absolute,      4}},
  {0xEE, {INC, Absolute,      6}},

  {0xF0, {BEQ, Relative,      3}},
  {0xF1, {SBC, IndirectPostY, 5}},
  {0xF5, {SBC, ZeroPageX,     4}},
  {0xF6, {INC, ZeroPageX,     6}},
  {0xF8, {SED, Implied,       2}},
  {0xF9, {SBC, AbsoluteY,     4}},
  {0xFD, {SBC, AbsoluteX,     4}},
  {0xFE, {INC, AbsoluteX,     7}},
};
// clang-format on

Instruction get_instruction(byte opcode) {
  // Throws an exception if opcode not found
  return INSTRUCTIONS.at(opcode);
}

bool is_read_instruction(Function function) {
  switch (function) {
    case LDA:
    case LDX:
    case LDY:
    case EOR:
    case AND:
    case ORA:
    case ADC:
    case SBC:
    case CMP:
      return true;

    default:
      return false;
  }
}

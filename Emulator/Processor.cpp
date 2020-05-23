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

#include "Instructions.h"

const int kCarryBit =
    0;  // Set when the result of the computation is < 0 or > 255 (unsigned)
const int kZeroBit = 1;  // Set when the result of the computation is 0
const int kInterruptBit = 2;
const int kDecimalBit = 3;  // unused in the NES version of the 6502
const int kBreakBit = 4;
const int kUnusedBit = 5;
const int kOverflowBit = 6;  // Set when there is a 2's complement overflow
const int kSignBit = 7;  // Indicates the sign of the result (set when negative)

const int kCarryMask = 1 << kCarryBit;
const int kZeroMask = 1 << kZeroBit;
const int kInterruptMask = 1 << kInterruptBit;
const int kDecimalMask = 1 << kDecimalBit;
const int kBreakMask = 1 << kBreakBit;
const int kUnusedMask = 1 << kUnusedBit;
const int kOverflowMask = 1 << kOverflowBit;
const int kSignMask = 1 << kSignBit;

Processor::Processor(PPU* ppu, ControllerPad* controller_pad)
    : ppu(ppu),
      controller_pad(controller_pad),
      cpu_ram(),
      sram(),
      prg_rom(nullptr) {}

void Processor::set_prg_rom(std::unique_ptr<byte[]> prg_rom) {
  this->prg_rom = std::move(prg_rom);
}

void Processor::reset() {
  pc = address_at(0xFFFC);  // address to jump to after reset

  s = 0xFF;
  a = 0;
  p = 1 << kUnusedBit;
  x = 0;
  y = 0;
}

void Processor::non_maskable_interrupt() {
  if (if_interrupt()) {
    stack_push(pc >> 8);
    stack_push(pc);
    stack_push(p & ~kBreakMask);  // NMI pushes 0 for break bit

    pc = address_at(0xFFFA);
  }
}

void Processor::set_p_bit(int bit, bool value) {
  value ? p |= 1 << bit : p &= ~(1 << bit);
}

/**
 * If result is nonnegative, set the carry bit. Otherwise, clear it.
 * This means that you should pass in only the higher-order byte(s) of any
 * calculation.
 */
void Processor::set_carry(byte result) { set_p_bit(kCarryBit, result); }
bool Processor::if_carry() { return p & kCarryMask; }

/**
 * If result is zero, set the zero bit. Otherwise, clear it.
 */
void Processor::set_zero(byte result) { set_p_bit(kZeroBit, !result); }
bool Processor::if_zero() { return p & kZeroMask; }

void Processor::set_interrupt(byte result) { set_p_bit(kInterruptBit, result); }
bool Processor::if_interrupt() { return p & kInterruptMask; }

void Processor::set_decimal(byte result) { set_p_bit(kDecimalBit, result); }
bool Processor::if_decimal() { return p & kDecimalMask; }

void Processor::set_break(byte result) { set_p_bit(kBreakBit, result); }
bool Processor::if_break() { return p & kBreakMask; }

/**
 * If result is nonzero, set the overflow bit. Otherwise, clear it.
 */
void Processor::set_overflow(byte result) { set_p_bit(kOverflowBit, result); }
bool Processor::if_overflow() { return p & kOverflowMask; }

void Processor::set_sign(byte result) {
  // we look at the 7th bit to determine the sign (this happens to be the same
  // bit as the sign bit in the control register)
  set_p_bit(kSignBit, result & kSignMask);
}
bool Processor::if_sign() { return p & kSignMask; }

dbyte Processor::address_at(dbyte memloc) {
  return read_memory(memloc + 1) << 8 | read_memory(memloc);
}

dbyte Processor::rel_addr(dbyte addr, byte offset) {
  // offset is actually a signed number. is there a better way to do this?
  if (offset & kSignMask) {
    return addr - static_cast<byte>(~offset + 1);
  } else {
    return addr + offset;
  }
}

void Processor::stack_push(byte value) { store_memory(0x100 + s--, value); }
byte Processor::stack_pop() { return read_memory(0x100 + ++s); }

byte Processor::read_memory(dbyte address) {
  if (address >= 0x8000) {
    // Program ROM space
    return prg_rom[address - 0x8000];
  } else if (address >= 0x6000) {
    // SRAM
    return sram[address - 0x6000];
  } else if (address >= 0x4020) {
    // Expansion ROM
    // throw "Expansion ROM not implemented";
  } else if (address >= 0x4000) {
    switch (address) {
      case 0x4016:
        return controller_pad->read_controller_1_state();
      case 0x4017:
        return controller_pad->read_controller_2_state();
      default:
        // Other I/O registers
        return 0;
    }
  } else if (address >= 0x2000) {
    // PPU I/O Registers
    switch (address & 0x07) {  // I/O registers are mirrored every 8 bytes
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

void Processor::store_memory(dbyte address, byte value) {
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
        ppu->write_spr_ram(cpu_ram + value * 0x100);
        break;
      case 0x4016:
        controller_pad->write_value(value);
        break;
      default:
        break;
        // throw "Unrecognized I/O write. Please implement.";
    }
  } else if (address >= 0x6000 && address < 0x8000) {
    // SRAM
    sram[address - 0x6000] = value;
  }
}

void Processor::execute() {
  byte opcode = read_memory(pc);  // opcode of instruction
  dbyte address;                  // address of operand
  byte src;                       // operand
  dbyte temp;                     // larger temp var for calculations

  Instruction instruction = get_instruction(opcode);

  /*** ADDRESSING MODE ***/
  switch (instruction.address_type) {
    case Immediate:
      address = pc + 1;
      pc += 2;
      break;

    case ZeroPage:
      address = read_memory(pc + 1);
      pc += 2;
      break;

    case ZeroPageX:
      address = (read_memory(pc + 1) + x) & 0xFF;  // Wrap-around addition
      pc += 2;
      break;

    case ZeroPageY:
      address = (read_memory(pc + 1) + y) & 0xFF;  // Wrap-around addition
      pc += 2;
      break;

    case Implied:
      pc += (opcode == 0x00 ? 2 : 1);  // BRK is two-byte opcode
      break;

    case Accumulator:
      src = a;
      pc += 1;
      break;

    case Absolute:
      address = address_at(pc + 1);
      pc += 3;
      break;

    case AbsoluteX:
      address = address_at(pc + 1) + x;
      pc += 3;
      break;

    case AbsoluteY:
      address = address_at(pc + 1) + y;
      pc += 3;
      break;

    case Indirect: {
      dbyte indirect_jump_address = address_at(pc + 1);
      if ((indirect_jump_address & 0xFF) == 0xFF) {
        // Wrap-around JMP bug
        address = (read_memory(indirect_jump_address & 0xFF00) << 8) +
                  read_memory(indirect_jump_address);
      } else {
        address = address_at(indirect_jump_address);
      }
      pc += 3;
      break;
    }

    case IndirectPreX:
      address =
          address_at((read_memory(pc + 1) + x) & 0xFF);  // Wrap-around addition
      pc += 2;
      break;

    case IndirectPostY: {
      byte op_address = read_memory(pc + 1);
      if (op_address == 0xFF) {
        throw "unhandled!";  // should wrap around
      }
      address = address_at(op_address) + y;
      pc += 2;
      break;
    }

    case Relative:
      address = pc + 1;
      pc += 2;
      break;
  }

  switch (instruction.function) {
    case ADC:
      src = read_memory(address);
      temp = a + src + (if_carry() ? 1 : 0);

      set_carry(temp > 0xFF);
      set_zero(temp & 0xFF);  // only look at last byte
      set_sign(temp);
      // If we add two numbers of the same sign and get a different sign,
      // there's overflow
      set_overflow(!((a ^ src) & kSignMask) && ((a ^ temp) & kSignMask));

      a = temp;
      break;

    case AND:
      a &= read_memory(address);
      set_zero(a);
      set_sign(a);
      break;

    case ASL:
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

    case BCC:
      src = read_memory(address);
      if (!if_carry()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BCS:
      src = read_memory(address);
      if (if_carry()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BEQ:
      src = read_memory(address);
      if (if_zero()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BIT:
      src = read_memory(address);
      set_overflow(src & kOverflowMask);
      set_sign(src);
      set_zero(a & src);
      break;

    case BMI:
      src = read_memory(address);
      if (if_sign()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BNE:
      src = read_memory(address);
      if (!if_zero()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BPL:
      src = read_memory(address);
      if (!if_sign()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BRK:
      stack_push(pc >> 8);
      stack_push(pc);
      set_break(1);
      stack_push(p | kBreakMask);
      set_interrupt(1);  // disable interrupts
      pc = address_at(0xFFFE);
      break;

    case BVC:
      src = read_memory(address);
      if (!if_overflow()) {
        pc = rel_addr(pc, src);
      }
      break;

    case BVS:
      src = read_memory(address);
      if (if_overflow()) {
        pc = rel_addr(pc, src);
      }
      break;

    case CLC:
      set_carry(0);
      break;

    case CLD:
      set_decimal(0);
      break;

    case CLI:
      set_interrupt(0);
      break;

    case CLV:
      set_overflow(0);
      break;

    case CMP:
      temp = a - read_memory(address);
      set_sign(temp);
      set_zero(temp);
      set_carry(temp < 0x100);  // if a > src, carry set
      break;

    case CPX:
      temp = x - read_memory(address);
      set_sign(temp);
      set_zero(temp);
      set_carry(temp < 0x100);  // if x > src, carry set
      break;

    case CPY:
      temp = y - read_memory(address);
      set_sign(temp);
      set_zero(temp);
      set_carry(temp < 0x100);  // if y > src, carry set
      break;

    case DEC:
      src = read_memory(address) - 1;
      set_zero(src);
      set_sign(src);
      store_memory(address, src);
      break;

    case DEX:
      x--;
      set_zero(x);
      set_sign(x);
      break;

    case DEY:
      y--;
      set_zero(y);
      set_sign(y);
      break;

    case EOR:
      a ^= read_memory(address);
      set_sign(a);
      set_zero(a);
      break;

    case INC:
      src = read_memory(address) + 1;
      set_sign(src);
      set_zero(src);
      store_memory(address, src);
      break;

    case INX:
      x++;
      set_sign(x);
      set_zero(x);
      break;

    case INY:
      y++;
      set_sign(y);
      set_zero(y);
      break;

    case JMP:
      pc = address;
      break;

    case JSR:
      pc--;
      stack_push(pc >> 8);  // Push the higher byte first
      stack_push(pc);
      pc = address;
      break;

    case LDA:
      a = read_memory(address);
      set_sign(a);
      set_zero(a);
      break;

    case LDX:
      x = read_memory(address);
      set_sign(x);
      set_zero(x);
      break;

    case LDY:
      y = read_memory(address);
      set_sign(y);
      set_zero(y);
      break;

    case LSR:
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

    case NOP:
      break;

    case ORA:
      a |= read_memory(address);
      set_sign(a);
      set_zero(a);
      break;

    case PHA:
      stack_push(a);
      break;

    case PHP:
      stack_push(p | kBreakMask);  // PHP sets the break flag
      break;

    case PLA:
      a = stack_pop();
      set_sign(a);
      set_zero(a);
      break;

    case PLP:
      p = stack_pop() | kUnusedMask;
      break;

    case ROL:
      if (opcode != 0x2A) src = read_memory(address);

      temp = src << 1;
      if (if_carry()) temp |= 0x01;
      set_carry(temp >> 8);
      src = temp;
      set_sign(src);
      set_zero(src);

      if (opcode == 0x2A) {
        a = src;
      } else {
        store_memory(address, src);
      }
      break;

    case ROR:
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

    case RTI:
      p = stack_pop();
      // Must make this two instructions so that the compiler doesn't screw us.
      pc = stack_pop();  // Pop the lower byte first
      pc |= stack_pop() << 8;
      break;

    case RTS:
      pc = stack_pop();  // Pop the lower byte first
      pc |= stack_pop() << 8;
      pc++;  // Must add 1
      break;

    case SBC:
      src = read_memory(address);
      temp = a - src - (if_carry() ? 0 : 1);
      set_zero(temp & 0xFF);
      set_sign(temp);
      set_overflow(((a ^ temp) & 0x80) && ((a ^ src) & 0x80));
      set_carry(temp < 0x100);
      a = temp;
      break;

    case SEC:
      set_carry(1);
      break;

    case SED:
      set_decimal(1);
      break;

    case SEI:
      set_interrupt(1);
      break;

    case STA:
      store_memory(address, a);
      break;

    case STX:
      store_memory(address, x);
      break;

    case STY:
      store_memory(address, y);
      break;

    case TAX:
      x = a;
      set_sign(x);
      set_zero(x);
      break;

    case TAY:
      y = a;
      set_sign(y);
      set_zero(y);
      break;

    case TSX:
      x = s;
      set_sign(x);
      set_zero(x);
      break;

    case TXA:
      a = x;
      set_sign(a);
      set_zero(a);
      break;

    case TXS:
      s = x;
      break;

    case TYA:
      a = y;
      set_sign(a);
      set_zero(a);
      break;
  }
}

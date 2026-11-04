#include "display.hpp"
#include "cpu.hpp"
#include <iostream>
#include "alu.hpp"
DecodedOpcode decode(Byte op){return {static_cast<Byte>((op>>4U)&0x0FU),static_cast<Byte>(op&0x0FU)};}
void dump_regs(const CPU& c){std::cout<<"PC="<<c.pc<<" H="<<c.h<<" A="<<static_cast<unsigned>(c.a)<<" B="<<static_cast<unsigned>(c.b)<<" Z="<<c.f.z<<" N="<<c.f.n<<" C="<<c.f.c<<'\n';}

#include "memory16.hpp"
#include <iomanip>

void step(CPU& c) {
    if (c.halted) { std::cout << "halted" << std::endl; return; }
    auto fail = [&](const char* message) { c.halted = true; std::cout << message << std::endl; };
    if (!c.mem || c.pc >= MEM_SIZE) { fail("invalid PC"); return; }
    const Byte op = mem_get(*c.mem, c.pc);
    auto fits = [&](unsigned size) {
        if (size > MEM_SIZE - c.pc) { fail("truncated instruction"); return false; }
        return true;
    };
    auto valid = [&](std::uint16_t address) {
        if (address >= MEM_SIZE) { fail("invalid address"); return false; }
        return true;
    };
    auto jump = [&](bool take) {
        if (!fits(3)) return;
        const auto target = get16(*c.mem, c.pc + 1);
        if (take) { if (valid(target)) c.pc = target; }
        else c.pc += 3;
    };
    switch (op) {
    case 0x00: c.halted = true; break;
    case 0x01: break;
    case 0x02: std::cout << static_cast<char>(c.a); break;
    case 0x03: std::cout << static_cast<unsigned>(c.a) << " "; break;
    case 0x04: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        for (std::size_t i = 0; i < 256; ++i) {
            if (i >= MEM_SIZE - address) { fail("unterminated string at memory boundary"); return; }
            const Byte value = mem_get(*c.mem, address + i);
            if (value == 0) break;
            std::cout << static_cast<char>(value);
        }
        c.pc += 3; return;
    }
    case 0x10: c.a = alu_add(c.a, c.b, c.f); break;
    case 0x11: c.a = alu_sub(c.a, c.b, c.f); break;
    case 0x12: c.a = alu_and(c.a, c.b, c.f); break;
    case 0x13: c.a = alu_or(c.a, c.b, c.f); break;
    case 0x14: c.a = alu_xor(c.a, c.b, c.f); break;
    case 0x15: c.a = alu_not(c.a, c.f); break;
    case 0x16: c.a = alu_shl(c.a, c.f); break;
    case 0x17: c.a = alu_shr(c.a, c.f); break;
    case 0x18: c.a = alu_inc(c.a, c.f); break;
    case 0x19: c.a = alu_dec(c.a, c.f); break;
    case 0x1a: (void)alu_sub(c.a, c.b, c.f); break;
    case 0x20:
        if (!fits(2)) return;
        c.a = mem_get(*c.mem, c.pc + 1); c.pc += 2; return;
    case 0x21:
        if (!fits(2)) return;
        c.b = mem_get(*c.mem, c.pc + 1); c.pc += 2; return;
    case 0x22: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        c.a = mem_get(*c.mem, address); c.pc += 3; return;
    }
    case 0x23: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        c.b = mem_get(*c.mem, address); c.pc += 3; return;
    }
    case 0x24: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        mem_set(*c.mem, address, c.a); c.pc += 3; return;
    }
    case 0x25: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        mem_set(*c.mem, address, c.b); c.pc += 3; return;
    }
    case 0x26: c.a = c.b; break;
    case 0x27: c.b = c.a; break;
    case 0x28:
        if (!fits(3)) return;
        c.h = get16(*c.mem, c.pc + 1); c.pc += 3; return;
    case 0x29:
        if (!valid(c.h)) return;
        c.a = mem_get(*c.mem, c.h); break;
    case 0x2a:
        if (!valid(c.h)) return;
        mem_set(*c.mem, c.h, c.a); break;
    case 0x2b: ++c.h; break;
    case 0x2c: --c.h; break;
    case 0x2d: c.a = static_cast<Byte>(c.h & 0xff); break;
    case 0x30: jump(true); return;
    case 0x31: jump(c.f.z); return;
    case 0x32: jump(!c.f.z); return;
    case 0x40: clear(*c.mem); break;
    case 0x41: c.f.c = !plot(*c.mem, c.a, c.b); break;
    case 0x43: show(*c.mem); break;
    default:
        c.halted = true;
        std::cout << "unknown opcode 0x" << std::hex << std::setw(2)
                  << std::setfill('0') << static_cast<unsigned>(op)
                  << std::dec << std::setfill(' ') << std::endl;
        return;
    }
    ++c.pc;
}

#include "display.hpp"
#include "cpu.hpp"
#include <iostream>
#include "alu.hpp"
DecodedOpcode decode(Byte op){return {static_cast<Byte>((op>>4U)&0x0FU),static_cast<Byte>(op&0x0FU)};}
void dump_regs(const CPU& c){std::cout<<"PC="<<c.pc<<" SP="<<c.stack.pointer()<<" H="<<c.h<<" A="<<static_cast<unsigned>(c.a)<<" B="<<static_cast<unsigned>(c.b)<<" Z="<<c.f.z<<" N="<<c.f.n<<" C="<<c.f.c<<'\n';}

#include "memory16.hpp"
#include <iomanip>

void step(CPU& c) {
    if (c.halted) { std::cout << "halted" << std::endl; return; }
    auto fail = [&](const char* message) { c.halted = true; std::cout << message << std::endl; };
    if (!c.mem || c.pc >= MEM_SIZE) { fail("invalid PC"); return; }
    const Op op = static_cast<Op>(mem_get(*c.mem, c.pc));
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
    case Op::Halt: c.halted = true; break;
    case Op::Nop: break;
    case Op::Out: std::cout << static_cast<char>(c.a); break;
    case Op::Outn: std::cout << static_cast<unsigned>(c.a) << " "; break;
    case Op::Outs: {
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
    case Op::Add: c.a = alu_add(c.a, c.b, c.f); break;
    case Op::Sub: c.a = alu_sub(c.a, c.b, c.f); break;
    case Op::And: c.a = alu_and(c.a, c.b, c.f); break;
    case Op::Or: c.a = alu_or(c.a, c.b, c.f); break;
    case Op::Xor: c.a = alu_xor(c.a, c.b, c.f); break;
    case Op::Not: c.a = alu_not(c.a, c.f); break;
    case Op::Shl: c.a = alu_shl(c.a, c.f); break;
    case Op::Shr: c.a = alu_shr(c.a, c.f); break;
    case Op::Inc: c.a = alu_inc(c.a, c.f); break;
    case Op::Dec: c.a = alu_dec(c.a, c.f); break;
    case Op::Cmp: (void)alu_sub(c.a, c.b, c.f); break;
    case Op::LoadiA:
        if (!fits(2)) return;
        c.a = mem_get(*c.mem, c.pc + 1); c.pc += 2; return;
    case Op::LoadiB:
        if (!fits(2)) return;
        c.b = mem_get(*c.mem, c.pc + 1); c.pc += 2; return;
    case Op::LoadA: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        c.a = mem_get(*c.mem, address); c.pc += 3; return;
    }
    case Op::LoadB: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        c.b = mem_get(*c.mem, address); c.pc += 3; return;
    }
    case Op::StoreA: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        mem_set(*c.mem, address, c.a); c.pc += 3; return;
    }
    case Op::StoreB: {
        if (!fits(3)) return;
        const auto address = get16(*c.mem, c.pc + 1);
        if (!valid(address)) return;
        mem_set(*c.mem, address, c.b); c.pc += 3; return;
    }
    case Op::MovAB: c.a = c.b; break;
    case Op::MovBA: c.b = c.a; break;
    case Op::LoadH:
        if (!fits(3)) return;
        c.h = get16(*c.mem, c.pc + 1); c.pc += 3; return;
    case Op::LoadAH:
        if (!valid(c.h)) return;
        c.a = mem_get(*c.mem, c.h); break;
    case Op::StoreHA:
        if (!valid(c.h)) return;
        mem_set(*c.mem, c.h, c.a); break;
    case Op::IncH: ++c.h; break;
    case Op::DecH: --c.h; break;
    case Op::HLow: c.a = static_cast<Byte>(c.h & 0xff); break;
    case Op::Jmp: jump(true); return;
    case Op::Jz: jump(c.f.z); return;
    case Op::Jnz: jump(!c.f.z); return;
    case Op::Cls: clear(*c.mem); break;
    case Op::Plot: c.f.c = !plot(*c.mem, c.a, c.b); break;
    case Op::Show: show(*c.mem); break;
    case Op::Mul: c.a = alu_mul(c.a, c.b, c.f); break;
    case Op::PushA:
        if (!c.stack.push(*c.mem,c.a)) { fail("stack overflow"); return; } break;
    case Op::PushB:
        if (!c.stack.push(*c.mem,c.b)) { fail("stack overflow"); return; } break;
    case Op::PopA:
        if (!c.stack.pop(*c.mem,c.a)) { fail("stack underflow"); return; } break;
    case Op::PopB:
        if (!c.stack.pop(*c.mem,c.b)) { fail("stack underflow"); return; } break;
    case Op::Call: {
        if (!fits(3)) return;
        const auto target=get16(*c.mem,c.pc+1);
        if (!valid(target)) return;
        if (!c.stack.push16(*c.mem,static_cast<std::uint16_t>(c.pc+3))) {
            fail("stack overflow"); return;
        }
        c.pc=target; return;
    }
    case Op::Ret: {
        std::uint16_t target=0;
        if (!c.stack.pop16(*c.mem,target)) { fail("stack underflow"); return; }
        if (!valid(target)) return;
        c.pc=target; return;
    }
    default:
        c.halted = true;
        std::cout << "unknown opcode 0x" << std::hex << std::setw(2)
                  << std::setfill('0') << static_cast<unsigned>(op)
                  << std::dec << std::setfill(' ') << std::endl;
        return;
    }
    ++c.pc;
}

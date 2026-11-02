#pragma once
#include "cpu.hpp"
Byte alu_add(Byte, Byte, Flags&); Byte alu_sub(Byte, Byte, Flags&);
Byte alu_and(Byte, Byte, Flags&); Byte alu_or(Byte, Byte, Flags&); Byte alu_xor(Byte, Byte, Flags&);
Byte alu_not(Byte, Flags&); Byte alu_shl(Byte, Flags&); Byte alu_shr(Byte, Flags&);
Byte alu_inc(Byte, Flags&); Byte alu_dec(Byte, Flags&);

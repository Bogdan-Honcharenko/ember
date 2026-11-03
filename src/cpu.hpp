#pragma once
#include <cstdint>
#include "memory.hpp"
struct Flags { bool z=false; bool n=false; bool c=false; };
struct CPU { Memory* mem=nullptr; std::uint16_t pc=0; std::uint16_t h=0; Byte a=0; Byte b=0; Flags f; bool halted=false; };
struct DecodedOpcode { Byte group; Byte index; };
DecodedOpcode decode(Byte op); void step(CPU&); void dump_regs(const CPU&);

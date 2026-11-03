#include "cpu.hpp"
#include <iostream>
#include "alu.hpp"
DecodedOpcode decode(Byte op){return {static_cast<Byte>((op>>4U)&0x0FU),static_cast<Byte>(op&0x0FU)};}
void dump_regs(const CPU& c){std::cout<<"PC="<<c.pc<<" H="<<c.h<<" A="<<static_cast<unsigned>(c.a)<<" B="<<static_cast<unsigned>(c.b)<<" Z="<<c.f.z<<" N="<<c.f.n<<" C="<<c.f.c<<'\n';}
void step_alu(CPU& c){if(c.halted){std::cout<<"halted\n";return;}if(c.mem==nullptr){std::cout<<"no memory\n";return;}Byte op=mem_get(*c.mem,c.pc);DecodedOpcode d=decode(op);if(d.group==0){if(d.index==0)c.halted=true;c.pc+=1;return;}if(d.group==1){switch(d.index){case 0:c.a=alu_add(c.a,c.b,c.f);break;case 1:c.a=alu_sub(c.a,c.b,c.f);break;case 2:c.a=alu_and(c.a,c.b,c.f);break;case 3:c.a=alu_or(c.a,c.b,c.f);break;case 4:c.a=alu_xor(c.a,c.b,c.f);break;case 5:c.a=alu_not(c.a,c.f);break;case 6:c.a=alu_shl(c.a,c.f);break;case 7:c.a=alu_shr(c.a,c.f);break;case 8:c.a=alu_inc(c.a,c.f);break;case 9:c.a=alu_dec(c.a,c.f);break;default:std::cout<<"unknown opcode: "<<static_cast<unsigned>(op)<<'\n';break;}c.pc+=1;return;}std::cout<<"unknown opcode: "<<static_cast<unsigned>(op)<<'\n';c.pc+=1;}

#include "cpu_memory.inc"

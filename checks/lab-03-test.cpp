#include "cpu.hpp"
#include "memory16.hpp"
#include <cassert>
#include <limits>
#include <iostream>
int main() {
 Memory m; CPU c; c.mem=&m;
 assert(set16(m,0,0x1234)); assert(m.data[0]==0x34 && m.data[1]==0x12);
 assert(get16(m,0)==0x1234); assert(set16(m,4094,65535));
 assert(!set16(m,4095,0)); assert(m.data[4095]==255);
 assert(!set16(m,std::numeric_limits<std::size_t>::max(),0));
 auto exec=[&](Byte op, unsigned operand=0){c.pc=0;c.halted=false;m.data[0]=op;set16(m,1,static_cast<std::uint16_t>(operand));c.f={true,true,true};step(c);assert(c.f.z&&c.f.n&&c.f.c);};
 exec(0x20,200);assert(c.a==200&&c.pc==2);
 exec(0x21,100);assert(c.b==100&&c.pc==2);
 exec(0x24,0x800);assert(m.data[0x800]==200&&c.pc==3);
 exec(0x25,0x801);assert(m.data[0x801]==100);
 exec(0x22,0x801);assert(c.a==100);
 exec(0x23,0x800);assert(c.b==200);
 exec(0x26);assert(c.a==200&&c.pc==1);
 c.a=66;exec(0x27);assert(c.b==66);
 exec(0x28,0x800);assert(c.h==0x800&&c.pc==3);
 exec(0x29);assert(c.a==200);c.a=65;exec(0x2a);assert(m.data[0x800]==65);
 exec(0x2b);assert(c.h==0x801);exec(0x2c);assert(c.h==0x800);
 exec(2);assert(c.pc==1);exec(3);assert(c.pc==1);
 exec(0);assert(c.halted&&c.pc==1);step(c);assert(c.pc==1);
 exec(0x22,4096);assert(c.halted&&c.pc==0);
 exec(0x24,65535);assert(c.halted);
 c.h=4096;exec(0x29);assert(c.halted);exec(0x2a);assert(c.halted);
 for(Byte op : {Byte(0x20),Byte(0x22),Byte(0x28)}) {c.pc=4095;c.halted=false;m.data[4095]=op;step(c);assert(c.halted&&c.pc==4095);}
 c.pc=4096;c.halted=false;step(c);assert(c.halted);
 c.mem=nullptr;c.pc=0;c.halted=false;step(c);assert(c.halted);
 std::cout << "LAB03 TESTS PASSED" << std::endl;
}

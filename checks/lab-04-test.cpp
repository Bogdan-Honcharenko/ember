#include "control.hpp"
#include "memory16.hpp"
#include <cassert>
#include <sstream>

int main() {
    Memory m; CPU c; c.mem = &m;
    std::ostringstream output; auto* saved = std::cout.rdbuf(output.rdbuf());
    for (unsigned a=0; a<256; ++a) for (unsigned b=0; b<256; ++b) {
        c.pc=0; c.halted=false; c.a=static_cast<Byte>(a); c.b=static_cast<Byte>(b); m.data[0]=0x1a;
        step(c); const Byte result=static_cast<Byte>(a-b);
        assert(c.a==a && c.b==b && c.pc==1);
        assert(c.f.z==(result==0) && c.f.n==((result&128)!=0) && c.f.c==(a<b));
    }
    for (Byte op : {Byte(0x30),Byte(0x31),Byte(0x32)}) for (bool z : {false,true}) {
        c.pc=10;c.halted=false;c.f={z,true,true};m.data[10]=op;set16(m,11,0x123);
        step(c);const bool taken=op==0x30 || (op==0x31 ? z : !z);
        assert(c.pc==(taken?0x123:13) && !c.halted);
        assert(c.f.z==z && c.f.n && c.f.c);
        c.pc=4094;c.halted=false;m.data[4094]=op;step(c);assert(c.halted && c.pc==4094);
    }
    c.pc=0;c.halted=false;m.data[0]=0x30;set16(m,1,4096);step(c);assert(c.halted&&c.pc==0);
    c.pc=0;c.halted=false;m.data[0]=0xff;step(c);assert(c.halted&&c.pc==0);
    assert(output.str().find("unknown opcode 0xff")!=std::string::npos);
    c.pc=0;c.halted=false;c.f={true,true,true};m.data[0]=0x20;m.data[1]=0;step(c);assert(c.f.z&&c.f.n&&c.f.c);
    c.pc=0;c.halted=false;m.data[0]=0x30;set16(m,1,0);run(c);assert(c.pc==0&&!c.halted);
    assert(output.str().find("step limit reached (100000)")!=std::string::npos);
    m.data[2048]=7;m.data[2049]=65;m.data[2050]=65;
    assert(find_byte(m,2048,2056,65)==2049);
    assert(find_byte(m,2048,2056,99)==2056);
    assert(find_byte(m,4096,4096,0)==4096);
    std::cout.rdbuf(saved);std::cout<<"LAB04 TESTS PASSED: CMP 65536 pairs, jumps, bounds, limit, search"<<std::endl;
}

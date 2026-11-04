#include "display.hpp"
#include "cpu.hpp"
#include "memory16.hpp"
#include <cassert>
#include <sstream>
#include <algorithm>

int main() {
    Memory mem;
    for (auto& b : mem.data) b=0x55;
    clear(mem);
    for (std::size_t i=0;i<MEM_SIZE;++i) assert(mem.data[i]==(i>=0xA00&&i<0xB00?0:0x55));
    std::ostringstream out;auto* saved=std::cout.rdbuf(out.rdbuf());
    for (int y=0;y<32;++y) for (int x=0;x<64;++x) {
        clear(mem); assert(plot(mem,x,y)); assert(plot(mem,x,y));
        out.str("");show(mem);const auto screen=out.str();
        assert(screen.size()==32*65);assert(std::count(screen.begin(),screen.end(),'#')==1);
        assert(screen[static_cast<std::size_t>(y)*65+x]=='#');
    }
    const Memory before=mem;
    assert(!plot(mem,-1,0)&&!plot(mem,0,-1)&&!plot(mem,64,0)&&!plot(mem,0,32));
    for(std::size_t i=0;i<MEM_SIZE;++i)assert(mem.data[i]==before.data[i]);
    clear(mem);plot(mem,0,0);plot(mem,7,0);plot(mem,8,0);
    assert(mem.data[0xA00]==0x81&&mem.data[0xA01]==0x80);
    frame(mem);out.str("");show(mem);const auto border=out.str();
    for(int y=0;y<32;++y)for(int x=0;x<64;++x)assert(border[y*65+x]==((x==0||x==63||y==0||y==31)?'#':'.'));
    CPU c;c.mem=&mem;
    auto exec=[&](Byte op){c.pc=0;c.halted=false;mem.data[0]=op;step(c);};
    c.f={true,true,true};exec(0x40);assert(c.pc==1&&c.f.z&&c.f.n&&c.f.c);
    c.a=13;c.b=2;exec(0x41);assert(c.pc==1&&!c.f.c&&c.f.z&&c.f.n&&mem.data[0xA11]==4);
    c.a=64;exec(0x41);assert(c.f.c&&c.f.z&&c.f.n&&mem.data[0xA11]==4);
    out.str("");exec(0x43);assert(out.str().size()==2080&&c.pc==1&&c.f.c);
    mem.data[0x800]='H';mem.data[0x801]='I';mem.data[0x802]=0;set16(mem,1,0x800);
    out.str("");exec(4);assert(out.str()=="HI"&&c.pc==3&&!c.halted&&c.f.z&&c.f.n&&c.f.c);
    for(unsigned i=0;i<257;++i)mem.data[0x800+i]='X';
    out.str("");exec(4);assert(out.str()==std::string(256,'X')&&c.pc==3&&!c.halted);
    set16(mem,1,4095);mem.data[4095]=0;out.str("");exec(4);assert(out.str().empty()&&c.pc==3);
    mem.data[4095]='Z';out.str("");exec(4);assert(c.halted&&c.pc==0&&out.str().find("memory boundary")!=std::string::npos);
    set16(mem,1,4096);exec(4);assert(c.halted&&c.pc==0);
    c.pc=4094;c.halted=false;mem.data[4094]=4;step(c);assert(c.halted&&c.pc==4094);
    clear(mem);assert(find_nonzero(mem,0xA00,0xB00)==0xB00);plot(mem,13,2);assert(find_nonzero(mem,0xA00,0xB00)==0xA11);
    std::cout.rdbuf(saved);std::cout<<"LAB05 TESTS PASSED: all 2048 pixels, frame, flags, bounds, OUTS limits"<<std::endl;
}

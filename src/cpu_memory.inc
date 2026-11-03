#include "memory16.hpp"
void step(CPU& c) {
 if(c.halted){std::cout<<"halted\n";return;}
 if(!c.mem||c.pc>=MEM_SIZE){c.halted=true;std::cout<<"invalid PC\n";return;}
 Byte op=mem_get(*c.mem,c.pc);
 if(op==2||op==3){if(op==2)std::cout<<static_cast<char>(c.a);else std::cout<<static_cast<unsigned>(c.a)<<' ';++c.pc;return;}
if(op<0x20 || op>0x2c){ if(op==0 || op==1 || (op>=0x10 && op<=0x19)) step_alu(c); else {c.halted=true;std::cout<<"unknown opcode"<<std::endl;} return; }
 unsigned size=(op==0x20||op==0x21)?2:((op>=0x22&&op<=0x25)||op==0x28)?3:1;
 if(size>MEM_SIZE-c.pc){c.halted=true;std::cout<<"truncated instruction\n";return;}
 std::uint16_t a=0;
 if(op>=0x22&&op<=0x25)a=get16(*c.mem,c.pc+1);
 if(op==0x29||op==0x2a)a=c.h;
 if(((op>=0x22&&op<=0x25)||op==0x29||op==0x2a)&&a>=MEM_SIZE){c.halted=true;std::cout<<"invalid data address\n";return;}
 switch(op){
 case 0x20:c.a=mem_get(*c.mem,c.pc+1);break;
 case 0x21:c.b=mem_get(*c.mem,c.pc+1);break;
 case 0x22:c.a=mem_get(*c.mem,a);break;
 case 0x23:c.b=mem_get(*c.mem,a);break;
 case 0x24:mem_set(*c.mem,a,c.a);break;
 case 0x25:mem_set(*c.mem,a,c.b);break;
 case 0x26:c.a=c.b;break;
 case 0x27:c.b=c.a;break;
 case 0x28:c.h=get16(*c.mem,c.pc+1);break;
 case 0x29:c.a=mem_get(*c.mem,a);break;
 case 0x2a:mem_set(*c.mem,a,c.a);break;
 case 0x2b:++c.h;break;
 case 0x2c:--c.h;break;
 }
 c.pc=static_cast<std::uint16_t>(c.pc+size);
}

#include "cpu.hpp"
#include "alu.hpp"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
const Byte fact[]={0x20,5,0x54,7,0,3,0,0x21,0,0x1a,0x31,0x1b,0,0x21,1,0x1a,0x31,0x1b,0,0x50,0x19,0x54,7,0,0x53,0x1b,0x55,0x20,1,0x55};
int main(){
 std::ostringstream quiet;auto* out=std::cout.rdbuf(quiet.rdbuf());
 for(unsigned a=0;a<256;++a)for(unsigned b=0;b<256;++b){Flags f{};Byte r=alu_mul(a,b,f);assert(r==((a*b)&255));assert(f.c==(a*b>255));assert(f.z==(r==0));assert(f.n==((r&128)!=0));}
 Memory m{};Stack s;Byte b=77;std::uint16_t word=123;
 assert(!s.pop(m,b)&&b==77);assert(!s.pop16(m,word)&&word==123);
 for(unsigned i=0;i<256;++i)assert(s.push(m,static_cast<Byte>(i)));
 assert(s.pointer()==0xEFF);assert(!s.push(m,1));assert(m.data[0xEFF]==0);
 for(int i=255;i>=0;--i){assert(s.pop(m,b));assert(b==i);}
 assert(s.pointer()==0xFFF);
 for(int i=0;i<255;++i)assert(s.push(m,7));
 auto before=m;assert(!s.push16(m,0x1234));assert(s.pointer()==0xF00);assert(std::memcmp(m.data,before.data,MEM_SIZE)==0);
 Stack one;assert(one.push(m,17));assert(!one.pop16(m,word));assert(one.pointer()==0xFFE);
 CPU c{};c.mem=&m;c.pc=0x123;m.data[0x123]=0x54;m.data[0x124]=0x56;m.data[0x125]=4;m.data[0x456]=0x55;c.f={true,true,true};
 step(c);assert(c.pc==0x456&&c.stack.pointer()==0xFFD);assert(m.data[0xFFF]==0x26&&m.data[0xFFE]==1);step(c);assert(c.pc==0x126&&c.stack.pointer()==0xFFF);assert(c.f.z&&c.f.n&&c.f.c);
 for(Byte op : {0x50,0x52}){c=CPU{};c.mem=&m;c.a=42;c.b=91;m.data[0]=op;step(c);assert(m.data[0xFFF]==(op==0x50?42:91));m.data[1]=(op==0x50?0x53:0x51);step(c);assert(c.stack.pointer()==0xFFF);assert((op==0x50?c.b:c.a)==(op==0x50?42:91));}
 for(Byte op : {0x51,0x53,0x55}){c=CPU{};c.mem=&m;m.data[0]=op;step(c);assert(c.halted&&c.pc==0&&c.stack.pointer()==0xFFF);}
 c=CPU{};c.mem=&m;m.data[0]=0x54;m.data[1]=0;m.data[2]=0x10;step(c);assert(c.halted&&c.stack.pointer()==0xFFF);
 c=CPU{};c.mem=&m;c.pc=0xFFE;m.data[0xFFE]=0x54;step(c);assert(c.halted&&c.stack.pointer()==0xFFF);
 std::ofstream trace("checks/lab-07-trace.txt");
 for(unsigned n : {0,1,5,6}){
  m=Memory{};std::memcpy(m.data,fact,sizeof(fact));m.data[1]=n;c=CPU{};c.mem=&m;unsigned steps=0;auto minsp=c.stack.pointer();
  while(!c.halted&&steps<1000){const auto pc=c.pc;const Byte op=m.data[pc];const auto sp=c.stack.pointer();const auto a=c.a;step(c);++steps;if(c.stack.pointer()<minsp)minsp=c.stack.pointer();
   if(n==5&&(op==0x54||op==0x55))trace<<std::hex<<"PC="<<pc<<" op="<<unsigned(op)<<" SP="<<sp<<" -> "<<c.stack.pointer()<<" A="<<std::dec<<unsigned(a)<<" -> "<<unsigned(c.a)<<" next PC="<<std::hex<<c.pc<<'\n';
  }
  assert(c.halted&&c.stack.pointer()==0xFFF);assert(c.a==(n<2?1:n==5?120:208));if(n==5){assert(steps==60);assert(minsp==0xFF1);}if(n==6)assert(c.f.c);
  trace<<std::dec<<"fact("<<n<<")="<<unsigned(c.a)<<" steps="<<steps<<" minSP="<<std::hex<<minsp<<" finalSP="<<c.stack.pointer()<<'\n';
 }
 m=Memory{};m.data[0]=0x54;c=CPU{};c.mem=&m;unsigned calls=0;while(!c.halted&&calls<200){step(c);++calls;}assert(c.halted&&calls==129&&c.stack.pointer()==0xEFF);
 assert(quiet.str().find("stack overflow")!=std::string::npos);assert(quiet.str().find("stack underflow")!=std::string::npos);
 std::cout.rdbuf(out);std::cout<<"LAB07 TESTS PASSED: 65536 MUL pairs, 256-byte stack, byte order, bounds, factorials, overflow\n";
}

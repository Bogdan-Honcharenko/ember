#include "cpu.hpp"
#include "sprite.hpp"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <type_traits>
int main() {
 static_assert(sizeof(Op)==1);
 static_assert(!std::is_convertible<Op,int>::value);
 std::cout << "CPU size=" << sizeof(CPU) << " align=" << alignof(CPU) << '\n';
#define FIELD(name) std::cout << #name << " offset=" << offsetof(CPU,name) << " size=" << sizeof(CPU::name) << '\n'
 FIELD(mem); FIELD(pc); FIELD(h); FIELD(a); FIELD(b); FIELD(f); FIELD(halted);
 std::cout << "Flags=" << sizeof(Flags) << " Sprite=" << sizeof(Sprite) << " Sprite align=" << alignof(Sprite) << '\n';
 std::cout << "Sprite offsets=" << offsetof(Sprite,x) << ',' << offsetof(Sprite,y) << ',' << offsetof(Sprite,vx) << ',' << offsetof(Sprite,vy) << ',' << offsetof(Sprite,alive) << '\n';
 for(int edge : {31,63}) for(int p=0;p<=edge;++p) for(int v=-127;v<=127;++v) {
  auto x=static_cast<std::uint8_t>(p); auto speed=static_cast<std::int8_t>(v);
  int expected=p, direction=v<0?-1:1;
  for(int n=0;n<std::abs(v);++n){if(expected+direction<0||expected+direction>edge)direction=-direction;expected+=direction;}
  move_axis(x,speed,edge);
  assert(x==expected);assert(speed==direction*std::abs(v));
 }
 Memory mem{};mem.data[0]=42;mem.data[0xB00]=73;
 Sprite sprites[8]{};sprites[0]={63,31,1,1,true};sprites[1]={0,0,-1,-1,true};
 std::ostringstream output;auto* old=std::cout.rdbuf(output.rdbuf());
 tick(mem,sprites);
 assert(sprites[0].x==62&&sprites[0].y==30&&sprites[0].vx==-1);
 assert(sprites[1].x==1&&sprites[1].y==1&&sprites[1].vy==1);
 assert(mem.data[0]==42&&mem.data[0xB00]==73);
 assert(output.str().size()==32*65);
 std::istringstream bad("8 1 1 1 1");sprite_command("sprite",bad,mem,sprites);
 assert(!sprites[7].alive);
 std::cout.rdbuf(old);
 std::cout<<"LAB06 TESTS PASSED: every position and speed, corners, commands, preserved memory\n";
}

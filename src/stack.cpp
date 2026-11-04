#include "stack.hpp"
#include <iomanip>
#include <ostream>
namespace { constexpr std::uint16_t low=0xF00, high=0xFFF; }
bool Stack::push(Memory& mem, Byte value) {
    if(sp_<low || sp_>high) return false;
    mem.data[sp_--]=value;
    return true;
}
bool Stack::pop(const Memory& mem, Byte& value) {
    if(sp_<low-1 || sp_>=high) return false;
    value=mem.data[++sp_];
    return true;
}
bool Stack::push16(Memory& mem, std::uint16_t value) {
    if(sp_<low+1 || sp_>high) return false;
    push(mem,static_cast<Byte>(value));
    push(mem,static_cast<Byte>(value>>8));
    return true;
}
bool Stack::pop16(const Memory& mem, std::uint16_t& value) {
    if(sp_<low-1 || sp_>high-2) return false;
    Byte hi=0,lo=0;
    pop(mem,hi);pop(mem,lo);
    value=static_cast<std::uint16_t>((hi<<8)|lo);
    return true;
}
void Stack::dump(const Memory& mem,std::ostream& out) const {
    const auto flags=out.flags(); const auto fill=out.fill();
    out<<"SP=0x"<<std::hex<<std::setw(3)<<std::setfill('0')<<sp_<<" top:";
    for(unsigned i=sp_+1;i<=high && i<=static_cast<unsigned>(sp_)+8;++i)
        out<<' '<<std::setw(3)<<i<<':'<<std::setw(2)<<static_cast<unsigned>(mem.data[i]);
    if(sp_==high)out<<" empty";
    out<<'\n';out.flags(flags);out.fill(fill);
}

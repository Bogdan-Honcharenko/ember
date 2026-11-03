#pragma once
#include "memory.hpp"
inline std::uint16_t get16(const Memory& m, std::size_t a) {
    if (a >= MEM_SIZE - 1) return 0;
    return static_cast<std::uint16_t>(mem_get(m,a) | (mem_get(m,a+1) << 8));
}
inline bool set16(Memory& m, std::size_t a, std::uint16_t v) {
    if (a >= MEM_SIZE - 1) return false;
    mem_set(m,a,static_cast<Byte>(v));
    mem_set(m,a+1,static_cast<Byte>(v >> 8));
    return true;
}

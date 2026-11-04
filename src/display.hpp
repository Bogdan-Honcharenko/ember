#pragma once
#include "memory.hpp"
#include <iostream>
#include <sstream>
#include <string>

inline constexpr std::size_t VRAM_LO = 0xA00;
inline constexpr std::size_t VRAM_SIZE = 256;
inline bool plot(Memory& mem, int x, int y) {
    if (x < 0 || x >= 64 || y < 0 || y >= 32) return false;
    const auto address = VRAM_LO + y * 8 + x / 8;
    mem.data[address] |= static_cast<Byte>(1u << (7 - x % 8));
    return true;
}
inline void clear(Memory& mem) {
    for (std::size_t i = 0; i < VRAM_SIZE; ++i) mem.data[VRAM_LO + i] = 0;
}
// Based on the show() scaffold supplied in Lab 05.
inline void show(const Memory& mem) {
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            const Byte cell = mem.data[VRAM_LO + y * 8 + x / 8];
            std::cout << (((cell >> (7 - x % 8)) & 1) ? '#' : '.');
        }
        std::cout << std::endl;
    }
}
inline void frame(Memory& mem) {
    clear(mem);
    for (int x = 0; x < 64; ++x) { plot(mem, x, 0); plot(mem, x, 31); }
    for (int y = 1; y < 31; ++y) { plot(mem, 0, y); plot(mem, 63, y); }
}
inline std::size_t find_nonzero(const Memory& mem, std::size_t lo, std::size_t hi) {
    for (std::size_t i = lo; i < hi; ++i) if (mem_get(mem, i) != 0) return i;
    return hi;
}
inline bool display_command(const std::string& cmd, std::istringstream& words, Memory& mem) {
    if (cmd == "show") { show(mem); return true; }
    if (cmd == "cls") { clear(mem); return true; }
    if (cmd == "frame") { frame(mem); return true; }
    if (cmd != "plot" && cmd != "findnz") return false;
    try {
        auto number = [&]() {
            std::string s; std::size_t used=0; if (!(words >> s)) throw 0;
            long value=std::stol(s,&used,0); if (used!=s.size()) throw 0; return value;
        };
        const long a=number(), b=number(); std::string extra;
        if (words >> extra) throw 0;
        if (cmd == "plot") {
            if (a<0 || a>=64 || b<0 || b>=32) throw 0;
            plot(mem,static_cast<int>(a),static_cast<int>(b));
        } else {
            if (a<0 || b<a || b>static_cast<long>(MEM_SIZE)) throw 0;
            const auto end=static_cast<std::size_t>(b);
            const auto found=find_nonzero(mem,static_cast<std::size_t>(a),end);
            if (found==end) std::cout << "not found" << std::endl;
            else std::cout << "found at " << found << std::endl;
        }
    } catch (...) {
        std::cout << (cmd=="plot" ? "usage: plot <0..63> <0..31>" : "usage: findnz <lo> <hi>; 0 <= lo <= hi <= 4096") << std::endl;
    }
    return true;
}

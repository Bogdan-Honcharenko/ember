#pragma once
#include "cpu.hpp"
#include <iostream>
#include <sstream>
#include <string>

inline constexpr unsigned RUN_LIMIT = 100000;
inline void run(CPU& cpu) {
    if (cpu.halted) { std::cout << "halted" << std::endl; return; }
    for (unsigned steps = 0; steps < RUN_LIMIT && !cpu.halted; ++steps) {
        step(cpu);
    }
    if (!cpu.halted) std::cout << "step limit reached (100000)" << std::endl;
}

inline std::size_t find_byte(const Memory& mem, std::size_t lo, std::size_t hi, Byte needle) {
    for (std::size_t i = lo; i < hi; ++i) {
        if (mem_get(mem, i) == needle) return i;
    }
    return hi;
}

inline bool find_command(const std::string& cmd, std::istringstream& words, const Memory& mem) {
    if (cmd != "find") return false;
    auto number = [&]() {
        std::string s; std::size_t used = 0;
        if (!(words >> s)) throw 0;
        long value = std::stol(s, &used, 0);
        if (used != s.size()) throw 0;
        return value;
    };
    try {
        const long lo = number(), hi = number(), needle = number();
        std::string extra;
        if (lo < 0 || hi < lo || hi > static_cast<long>(MEM_SIZE) || needle < 0 || needle > 255 || (words >> extra)) throw 0;
        const auto result = find_byte(mem, static_cast<std::size_t>(lo), static_cast<std::size_t>(hi), static_cast<Byte>(needle));
        if (result == static_cast<std::size_t>(hi)) std::cout << "not found" << std::endl;
        else std::cout << "found at " << result << std::endl;
    } catch (...) {
        std::cout << "usage: find <lo> <hi> <byte>; 0 <= lo <= hi <= 4096, byte 0..255" << std::endl;
    }
    return true;
}

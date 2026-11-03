#pragma once
#include "memory16.hpp"
#include <iostream>
#include <sstream>
#include <string>
inline bool memory_command(const std::string& cmd, std::istringstream& words, Memory& mem) {
    if (cmd != "get16" && cmd != "set16") return false;
    std::string token;
    try {
        if (!(words >> token)) throw 0;
        std::size_t used = 0;
        long address = std::stol(token, &used, 0);
        if (used != token.size() || address < 0 || address >= static_cast<long>(MEM_SIZE - 1)) throw 0;
        auto index = static_cast<std::size_t>(address);
        if (cmd == "get16") std::cout << get16(mem, index) << std::endl;
        else {
            if (!(words >> token)) throw 0;
            long value = std::stol(token, &used, 0);
            if (used != token.size() || value < 0 || value > 65535) throw 0;
            set16(mem, index, static_cast<std::uint16_t>(value));
        }
    } catch (...) {
        std::cout << "usage: " << cmd << " <0..4094>" << (cmd == "set16" ? " <0..65535>" : "") << std::endl;
    }
    return true;
}

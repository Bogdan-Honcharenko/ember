#pragma once
#include "memory.hpp"
#include <cstdint>
#include <iosfwd>

class Stack {
public:
    bool push(Memory& mem, Byte value);
    bool pop(const Memory& mem, Byte& value);
    bool push16(Memory& mem, std::uint16_t value);
    bool pop16(const Memory& mem, std::uint16_t& value);
    std::uint16_t pointer() const { return sp_; }
    void dump(const Memory& mem, std::ostream& out) const;
private:
    std::uint16_t sp_ = 0xFFF;
};

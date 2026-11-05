#include "assembler.hpp"
#include "cpu.hpp"
#include "lexer.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
int main() {
    std::string error;
    for (int i = 0; i < 200; ++i) {
        auto* list = lex("LOADI A, 0b101\n; comment\nHALT", error);
        assert(list && error.empty());
        int count = 0;
        for (auto* p = list; p; p = p->next)
            ++count;
        assert(count == 8);
        destroy(list);
        auto* bad = lex("NOP\nLOADI A, 0xGG", error);
        assert(!bad && error.find("line 2") != std::string::npos);
    }
    std::vector<Byte> bytes{99};
    assert(!assemble("JMP missing", bytes, error));
    assert(bytes == std::vector<Byte>{99});
    assert(assemble("LOADI A, 0xff\nJMP 0x123", bytes, error));
    assert((bytes == std::vector<Byte>{0x20, 0xff, 0x30, 0x23, 0x01}));
    for (const auto* name : {"fib", "fib10"}) {
        std::ifstream file(std::string("programs/") + name + ".asm");
        std::string source((std::istreambuf_iterator<char>(file)), {});
        assert(assemble(source, bytes, error));
        Memory mem{};
        for (std::size_t i = 0; i < bytes.size(); ++i)
            mem.data[i] = bytes[i];
        CPU cpu{};
        cpu.mem = &mem;
        unsigned steps = 0;
        while (!cpu.halted && steps++ < 100000)
            step(cpu);
        assert(cpu.halted && steps < 100000 && cpu.stack.pointer() == 0xfff);
        assert(cpu.a == (std::string(name) == "fib" ? 8 : 55));
    }
    std::cout << "\nLab 08: token ownership, atomic assembly, little-endian operands and recursive "
                 "stack balance PASS\n";
}

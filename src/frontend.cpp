#include "frontend.hpp"
#include "assembler.hpp"
#include "lexer.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>
namespace {
bool read(const std::string& path, std::string& source) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open: " << path << '\n';
        return false;
    }
    source.assign(std::istreambuf_iterator<char>(file), {});
    return true;
}
bool load(const std::string& path, Memory& mem, CPU& cpu) {
    std::string source, message;
    std::vector<Byte> bytes;
    if (!read(path, source))
        return false;
    if (!assemble(source, bytes, message)) {
        std::cerr << message << '\n';
        return false;
    }
    Memory image{};
    for (std::size_t i = 0; i < bytes.size(); ++i)
        image.data[i] = bytes[i];
    mem = image;
    cpu = CPU{};
    cpu.mem = &mem;
    return true;
}
void lex_print(const Token* token) {
    const char* names[] = {"Ident",    "Number",   "Comma",   "Colon",
                           "LBracket", "RBracket", "Newline", "Eof"};
    for (; token; token = token->next)
        std::cout << token->line << ' ' << names[static_cast<unsigned>(token->kind)] << ' '
                  << (token->kind == Token::Kind::Newline ? "\\n" : token->text) << '\n';
}
} // namespace
int assembly_cli(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        std::cerr << "usage: ember [file.asm | --lex file.asm | --asm file.asm]\n";
        return 2;
    }
    if (argc == 3) {
        const std::string option = argv[1];
        if (option != "--lex" && option != "--asm") {
            std::cerr << "unknown option\n";
            return 2;
        }
        std::string source, message;
        if (!read(argv[2], source))
            return 1;
        if (option == "--lex") {
            std::unique_ptr<Token, decltype(&destroy)> tokens(lex(source, message), destroy);
            if (!tokens) {
                std::cerr << message << '\n';
                return 1;
            }
            lex_print(tokens.get());
            return 0;
        }
        std::vector<Byte> bytes;
        if (!assemble(source, bytes, message)) {
            std::cerr << message << '\n';
            return 1;
        }
        for (std::size_t i = 0; i < bytes.size(); ++i) {
            if (i % 16 == 0)
                std::cout << std::hex << std::setfill('0') << std::setw(4) << i << ":";
            std::cout << ' ' << std::setw(2) << unsigned(bytes[i]);
            if (i % 16 == 15 || i + 1 == bytes.size())
                std::cout << '\n';
        }
        return 0;
    }
    Memory mem{};
    CPU cpu{};
    if (!load(argv[1], mem, cpu))
        return 1;
    for (unsigned count = 0; count < 100000; ++count) {
        const bool halt = cpu.pc < MEM_SIZE && mem.data[cpu.pc] == 0;
        step(cpu);
        if (cpu.halted)
            return halt ? 0 : 1;
    }
    std::cerr << "step limit reached\n";
    return 1;
}
bool assembly_command(const std::string& cmd, std::istringstream& words, Memory& mem, CPU& cpu) {
    if (cmd != "asm" && cmd != "lex")
        return false;
    std::string path, extra;
    if (!(words >> path) || (words >> extra)) {
        std::cout << "usage: " << cmd << " <file.asm>\n";
        return true;
    }
    if (cmd == "asm") {
        if (load(path, mem, cpu))
            std::cout << "assembled; PC=0, memory and CPU reset\n";
    } else {
        std::string source, message;
        if (read(path, source)) {
            std::unique_ptr<Token, decltype(&destroy)> tokens(lex(source, message), destroy);
            if (tokens)
                lex_print(tokens.get());
            else
                std::cerr << message << '\n';
        }
    }
    return true;
}

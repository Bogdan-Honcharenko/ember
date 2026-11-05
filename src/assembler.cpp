#include "assembler.hpp"
#include "cpu.hpp"
#include "lexer.hpp"
#include <cctype>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {
struct Spec {
    const char* name;
    const char* operands;
    Op op;
    unsigned size;
};
const Spec specs[] = {{"HALT", "", Op::Halt, 1},
                      {"NOP", "", Op::Nop, 1},
                      {"OUT", "", Op::Out, 1},
                      {"OUTN", "", Op::Outn, 1},
                      {"OUTS", "@", Op::Outs, 3},
                      {"ADD", "A , B", Op::Add, 1},
                      {"SUB", "A , B", Op::Sub, 1},
                      {"AND", "A , B", Op::And, 1},
                      {"OR", "A , B", Op::Or, 1},
                      {"XOR", "A , B", Op::Xor, 1},
                      {"NOT", "A", Op::Not, 1},
                      {"SHL", "A", Op::Shl, 1},
                      {"SHR", "A", Op::Shr, 1},
                      {"INC", "A", Op::Inc, 1},
                      {"DEC", "A", Op::Dec, 1},
                      {"CMP", "A , B", Op::Cmp, 1},
                      {"MUL", "A , B", Op::Mul, 1},
                      {"LOADI", "A , @", Op::LoadiA, 2},
                      {"LOADI", "B , @", Op::LoadiB, 2},
                      {"LOAD", "A , [ H ]", Op::LoadAH, 1},
                      {"STORE", "[ H ] , A", Op::StoreHA, 1},
                      {"LOAD", "A , [ @ ]", Op::LoadA, 3},
                      {"LOAD", "B , [ @ ]", Op::LoadB, 3},
                      {"STORE", "[ @ ] , A", Op::StoreA, 3},
                      {"STORE", "[ @ ] , B", Op::StoreB, 3},
                      {"MOV", "A , B", Op::MovAB, 1},
                      {"MOV", "B , A", Op::MovBA, 1},
                      {"LOADH", "H , @", Op::LoadH, 3},
                      {"INCH", "", Op::IncH, 1},
                      {"DECH", "", Op::DecH, 1},
                      {"HLOW", "", Op::HLow, 1},
                      {"JMP", "@", Op::Jmp, 3},
                      {"JZ", "@", Op::Jz, 3},
                      {"JNZ", "@", Op::Jnz, 3},
                      {"CLS", "", Op::Cls, 1},
                      {"PLOT", "", Op::Plot, 1},
                      {"SHOW", "", Op::Show, 1},
                      {"PUSH", "A", Op::PushA, 1},
                      {"PUSH", "B", Op::PushB, 1},
                      {"POP", "A", Op::PopA, 1},
                      {"POP", "B", Op::PopB, 1},
                      {"CALL", "@", Op::Call, 3},
                      {"RET", "", Op::Ret, 1}};
std::string upper(std::string s) {
    for (char& c : s)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
[[noreturn]] void error(int line, const std::string& message) {
    throw std::runtime_error("line " + std::to_string(line) + ": " + message);
}
unsigned numeric(const Token& t, unsigned limit) {
    unsigned base = 10, value = 0;
    std::size_t i = 0;
    if (t.text.size() > 2 && t.text[0] == '0') {
        if (t.text[1] == 'x' || t.text[1] == 'X') {
            base = 16;
            i = 2;
        }
        if (t.text[1] == 'b' || t.text[1] == 'B') {
            base = 2;
            i = 2;
        }
    }
    for (; i < t.text.size(); ++i) {
        char c = t.text[i];
        unsigned d =
            c >= '0' && c <= '9' ? c - '0' : std::tolower(static_cast<unsigned char>(c)) - 'a' + 10;
        if (d > limit || value > (limit - d) / base)
            error(t.line, "operand out of range: " + t.text);
        value = value * base + d;
    }
    return value;
}
struct Instruction {
    const Spec* spec;
    const Token* value;
    int line;
};
} // namespace
bool assemble(const std::string& source, std::vector<Byte>& bytes, std::string& message) {
    message.clear();
    std::unique_ptr<Token, decltype(&destroy)> tokens(lex(source, message), destroy);
    if (!tokens)
        return false;
    try {
        std::map<std::string, unsigned> labels;
        std::vector<Instruction> instructions;
        unsigned pc = 0;
        Token* cursor = tokens.get();
        while (cursor->kind != Token::Kind::Eof) {
            std::vector<const Token*> line;
            while (cursor->kind != Token::Kind::Newline && cursor->kind != Token::Kind::Eof) {
                line.push_back(cursor);
                cursor = cursor->next;
            }
            if (cursor->kind == Token::Kind::Newline)
                cursor = cursor->next;
            if (line.empty())
                continue;
            if (line.size() >= 2 && line[0]->kind == Token::Kind::Ident &&
                line[1]->kind == Token::Kind::Colon) {
                if (pc >= 0x800)
                    error(line[0]->line, "label outside code region");
                if (!labels.emplace(line[0]->text, pc).second)
                    error(line[0]->line, "duplicate label: " + line[0]->text);
                line.erase(line.begin(), line.begin() + 2);
            }
            if (line.empty())
                continue;
            const Spec* selected = nullptr;
            const Token* operand = nullptr;
            if (line[0]->kind != Token::Kind::Ident)
                error(line[0]->line, "expected instruction");
            for (const auto& spec : specs) {
                if (upper(line[0]->text) != spec.name)
                    continue;
                std::istringstream pattern(spec.operands);
                std::string part;
                std::size_t i = 1;
                bool match = true;
                const Token* candidate = nullptr;
                while (pattern >> part) {
                    if (i >= line.size()) {
                        match = false;
                        break;
                    }
                    if (part == "@") {
                        if (line[i]->kind != Token::Kind::Number &&
                            line[i]->kind != Token::Kind::Ident) {
                            match = false;
                            break;
                        }
                        candidate = line[i];
                    } else if (upper(line[i]->text) != part) {
                        match = false;
                        break;
                    }
                    ++i;
                }
                if (match && i == line.size()) {
                    selected = &spec;
                    operand = candidate;
                    break;
                }
            }
            if (!selected)
                error(line[0]->line, "unknown instruction or invalid operands: " + line[0]->text);
            if (pc + selected->size > 0x800)
                error(line[0]->line, "program exceeds code region (2048 bytes)");
            instructions.push_back({selected, operand, line[0]->line});
            pc += selected->size;
        }
        std::vector<Byte> result;
        result.reserve(pc);
        for (const auto& instruction : instructions) {
            unsigned value = 0;
            const unsigned limit = instruction.spec->size == 2 ? 255 : 4095;
            if (instruction.value) {
                const auto& t = *instruction.value;
                if (t.kind == Token::Kind::Number)
                    value = numeric(t, limit);
                else {
                    auto it = labels.find(t.text);
                    if (it == labels.end())
                        error(t.line, "undefined label: " + t.text);
                    value = it->second;
                    if (value > limit)
                        error(t.line, "label out of operand range: " + t.text);
                }
            }
            result.push_back(static_cast<Byte>(instruction.spec->op));
            if (instruction.spec->size >= 2)
                result.push_back(static_cast<Byte>(value));
            if (instruction.spec->size == 3)
                result.push_back(static_cast<Byte>(value >> 8));
        }
        bytes.swap(result);
        return true;
    } catch (const std::runtime_error& e) {
        message = e.what();
        return false;
    }
}

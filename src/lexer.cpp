// lexer.cpp — ДАНО, крім number(). Прочитайте цикл у lex() згори донизу:
// це псевдокод з теорії §1, записаний на C++.
#include "lexer.hpp"

#include <cctype>

// Стан лексера: текст, курсор, номер рядка і список, який ми будуємо.
struct Lexer {
    const std::string& src;
    std::size_t i = 0;
    int line = 1;
    Token* head = nullptr;
    Token* tail = nullptr;
    std::string error; // непорожній, щойно лексер щось відхилив
};

// Додати вузол у кінець списку (Notes 08 §3).
static void emit(Lexer& lx, Token::Kind kind, const std::string& text) {
    Token* t = new Token{kind, text, lx.line, nullptr};
    if (!lx.head) {
        lx.head = lx.tail = t;
    } else {
        lx.tail->next = t;
        lx.tail = t;
    }
}

// Записати помилку з номером рядка. Повертає false, щоб зручно писати
// `return fail(lx, "...");`
static bool fail(Lexer& lx, const std::string& msg) {
    lx.error = "line " + std::to_string(lx.line) + ": " + msg;
    return false;
}

void destroy(Token* head) {
    while (head) {
        Token* next = head->next;
        delete head;
        head = next;
    }
}

// ВАШЕ (Lab 8, M1). Курсор lx.i стоїть на цифрі.
// З'їсти число — `0x` і шістнадцяткові цифри, `0b` і двійкові, або десяткові —
// і видати його: emit(lx, Token::Kind::Number, текст_числа).
// Зіпсоване число — `0xGG`, `0x` без жодної цифри, `12ab` — це
// `return fail(lx, "bad number");`, а не тихий нуль.
static bool number(Lexer& lx) {
    const auto start = lx.i;
    while (lx.i < lx.src.size() &&
           (std::isalnum(static_cast<unsigned char>(lx.src[lx.i])) || lx.src[lx.i] == '_'))
        ++lx.i;
    const auto text = lx.src.substr(start, lx.i - start);
    unsigned base = 10;
    std::size_t digit = 0;
    if (text.size() >= 2 && text[0] == '0') {
        if (text[1] == 'x' || text[1] == 'X') {
            base = 16;
            digit = 2;
        } else if (text[1] == 'b' || text[1] == 'B') {
            base = 2;
            digit = 2;
        }
    }
    if (digit == text.size())
        return fail(lx, "missing digits in '" + text + "'");
    for (; digit < text.size(); ++digit) {
        const char c = text[digit];
        unsigned value = c >= '0' && c <= '9'   ? c - '0'
                         : c >= 'a' && c <= 'f' ? c - 'a' + 10
                         : c >= 'A' && c <= 'F' ? c - 'A' + 10
                                                : 99;
        if (value >= base)
            return fail(lx, "invalid number '" + text + "'");
    }
    emit(lx, Token::Kind::Number, text);
    return true;
}

Token* lex(const std::string& src, std::string& error) {
    error.clear();
    Lexer lx{src, 0, 1, nullptr, nullptr, ""};
    while (lx.i < src.size()) {
        char c = src[lx.i];
        if (c == ' ' || c == '\t' || c == '\r') {
            ++lx.i; // пробіли не токени
        } else if (c == ';') {
            while (lx.i < src.size() && src[lx.i] != '\n')
                ++lx.i; // коментар
        } else if (c == '\n') {
            emit(lx, Token::Kind::Newline, "\\n");
            ++lx.line;
            ++lx.i;
        } else if (std::isalpha((unsigned char)c) || c == '_') {
            std::size_t start = lx.i;
            while (lx.i < src.size() &&
                   (std::isalnum((unsigned char)src[lx.i]) || src[lx.i] == '_')) {
                ++lx.i;
            }
            emit(lx, Token::Kind::Ident, src.substr(start, lx.i - start));
        } else if (std::isdigit((unsigned char)c)) {
            if (!number(lx))
                break;
        } else if (c == ',') {
            emit(lx, Token::Kind::Comma, ",");
            ++lx.i;
        } else if (c == ':') {
            emit(lx, Token::Kind::Colon, ":");
            ++lx.i;
        } else if (c == '[') {
            emit(lx, Token::Kind::LBracket, "[");
            ++lx.i;
        } else if (c == ']') {
            emit(lx, Token::Kind::RBracket, "]");
            ++lx.i;
        } else {
            fail(lx, std::string("unexpected character '") + c + "'");
            break;
        }
    }
    if (!lx.error.empty()) {
        destroy(lx.head);
        error = lx.error;
        return nullptr;
    }
    emit(lx, Token::Kind::Eof, "");
    return lx.head;
}

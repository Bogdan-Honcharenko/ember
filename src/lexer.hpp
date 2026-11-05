#pragma once
#include <string>
struct Token {
    enum class Kind { Ident, Number, Comma, Colon, LBracket, RBracket, Newline, Eof };
    Kind kind;
    std::string text;
    int line;
    Token* next;
};
Token* lex(const std::string& src, std::string& error);
void destroy(Token* head);

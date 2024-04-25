#pragma once

#include <vector>
#include <memory>

#include "lex.h"
#include "expr.h"

class ParseError;

class Parser 
{
private:
    const std::vector<Token>& m_tokens;
    int current = 0;

    template <class... T>
    bool match(T... type);

    bool  check(TokenType type);
    Token advance();
    bool  is_at_end();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string message);
    void synchronize();
    ParseError* error(Token token, std::string message);

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();
public:
    Parser(const std::vector<Token>& tokens)
        : m_tokens{tokens}
    { }
    ~Parser() = default;

    std::shared_ptr<Expr> parse();
};

class ParseError
{
public:
    ParseError() { }
    ~ParseError() = default;
};

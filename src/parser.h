#pragma once

#include <vector>
#include <memory>
#include <stdexcept>

#include "lex.h"
#include "expr.h"
#include "stmt.h"
#include "lox_return.h"

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
    ParseError error(Token token, std::string message);

    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> for_statement();
    std::shared_ptr<Stmt> if_statement();
    std::shared_ptr<Stmt> while_statement();
    std::shared_ptr<Stmt> print_statement();
    std::shared_ptr<Stmt> return_statement();
    std::shared_ptr<Stmt> var_declaration();
    std::shared_ptr<Stmt> expression_statement();
    std::shared_ptr<Function> function(std::string kind);
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> Or();
    std::shared_ptr<Expr> And();
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> finish_call(std::shared_ptr<Expr> callee);
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();
public:
    Parser(const std::vector<Token>& tokens)
        : m_tokens{tokens} { }
    ~Parser() = default;

    std::vector<std::shared_ptr<Stmt>> parse();
};

class ParseError : public std::runtime_error 
{
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

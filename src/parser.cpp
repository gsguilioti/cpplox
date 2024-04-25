#include <stdexcept>

#include "parser.h"

void error(Token token, std::string message);

template <class... T>
bool Parser::match(T... type) 
{
    if ((... || check(type))) 
    {
      advance();
      return true;
    }

    return false;
}

bool Parser::check(TokenType type)
{
    if(is_at_end()) return false;

    return peek().m_type == type;
}

Token Parser::advance()
{
    if(!is_at_end()) current++;

    return previous();
}

bool Parser::is_at_end()
{
    return peek().m_type == EOF;
}

Token Parser::peek()
{
    return m_tokens[current];
}

Token Parser::previous()
{
    return m_tokens[current -1];
}

ParseError* Parser::error(Token token, std::string message)
{
    error(token, message);
    return new ParseError();
}

Token Parser::consume(TokenType type, std::string message)
{
    if(check(type)) return advance();

    throw error(peek(), message);
}

void Parser::synchronize()
{
    advance();

    while(!is_at_end())
    {
        if(previous().m_type == TokenType::SEMICOLON) return;

        switch(peek().m_type)
        {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
        }

        advance();
    }
}

std::shared_ptr<Expr> Parser::parse()
{
    try
    {
        return expression();
    }
    catch(ParseError e)
    {
        return nullptr;
    }
    
}

std::shared_ptr<Expr> Parser::expression()
{  
    return equality();
}

std::shared_ptr<Expr> Parser::equality()
{
    std::shared_ptr<Expr> expr = comparison();

    while(match(BANG_EQUAL, EQUAL_EQUAL))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison()
{
    std::shared_ptr<Expr> expr = term();

    while(match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term()
{
    std::shared_ptr<Expr> expr = factor();

    while(match(MINUS, PLUS))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor()
{
    std::shared_ptr<Expr> expr = unary();

    while(match(SLASH, STAR))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary()
{
    if(match(BANG, MINUS))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary>(op, right);
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary()
{
    if (match(FALSE)) return std::make_shared<Literal>(false);
    if (match(TRUE)) return std::make_shared<Literal>(true);
    if (match(NIL)) return std::make_shared<Literal>(nullptr);

    if (match(NUMBER, STRING)) {
      return std::make_shared<Literal>(previous().m_literal);
    }

    if (match(LEFT_PAREN)) {
      std::shared_ptr<Expr> expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expect expression.");
}

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
    if(is_at_end()) 
        return false;

    return peek().m_type == type;
}

Token Parser::advance()
{
    if(!is_at_end()) 
        ++current;

    return previous();
}

bool Parser::is_at_end()
{
    return peek().m_type == END;
}

Token Parser::peek()
{
    return m_tokens.at(current);
}

Token Parser::previous()
{
    return m_tokens.at(current - 1);
}

ParseError Parser::error(Token token, std::string message)
{
    error(token, message);
    return ParseError(message);
}

Token Parser::consume(TokenType type, std::string message)
{
    if(check(type)) 
        return advance();

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

std::vector<std::shared_ptr<Stmt>> Parser::parse()
{
    std::vector<std::shared_ptr<Stmt>> statements;
    while(!is_at_end())
        statements.push_back(declaration());

    return statements;
}

std::shared_ptr<Stmt> Parser::statement()
{
    if(match(PRINT)) return print_statement();
    if(match(LEFT_BRACE)) return std::make_shared<Block>(block());

    return expression_statement();
}

std::shared_ptr<Stmt> Parser::print_statement()
{
    std::shared_ptr<Expr> value = expression();

    consume(SEMICOLON, "Expect ';' after value.");

    return std::make_shared<Print>(value);
}

std::shared_ptr<Stmt> Parser::var_declaration()
{
    Token name = consume(IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if(match(EQUAL))
        initializer = expression();

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Var>(name, initializer);
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
    std::shared_ptr<Expr> value = expression();

    consume(SEMICOLON, "Expect ';' after value.");
    
    return std::make_shared<Expression>(value);
}

std::vector<std::shared_ptr<Stmt>> Parser::block()
{
    std::vector<std::shared_ptr<Stmt>> statements;

    while(!check(RIGHT_BRACE) && !is_at_end())
        statements.emplace_back(declaration());

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<Expr> Parser::assignment()
{  
    std::shared_ptr<Expr> expr = equality();

    if(match(EQUAL))
    {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment();

        if(auto var = std::dynamic_pointer_cast<Variable>(expr))
        {
            Token name = var->m_name;
            return std::make_shared<Assign>(name, value);
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::expression()
{  
    return assignment();
}

std::shared_ptr<Stmt> Parser::declaration()
{
    try
    {
        if(match(VAR)) return var_declaration();

        return statement();
    }
    catch(ParseError error)
    {
       synchronize();
        return nullptr;
    }
    
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

    if (match(NUMBER, STRING)) 
      return std::make_shared<Literal>(previous().m_literal);

    if(match(IDENTIFIER))
        return std::make_shared<Variable>(previous());

    if (match(LEFT_PAREN)) 
    {
      std::shared_ptr<Expr> expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expect expression.");
}

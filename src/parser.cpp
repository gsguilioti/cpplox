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
    if(match(FOR)) return for_statement();
    if(match(IF)) return if_statement();
    if(match(PRINT)) return print_statement();
    if (match(RETURN)) return return_statement();
    if(match(WHILE)) return while_statement();
    if(match(LEFT_BRACE)) return std::make_shared<Block>(block());

    return expression_statement();
}

std::shared_ptr<Stmt> Parser::for_statement()
{
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if (match(SEMICOLON))
      initializer = nullptr;
    else if (match(VAR))
      initializer = var_declaration();
    else 
      initializer = expression_statement();

    std::shared_ptr<Expr> condition = nullptr;
    if(!check(SEMICOLON))
        condition = expression();

    consume(SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if (!check(RIGHT_PAREN)) 
      increment = expression();

    consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    std::shared_ptr<Stmt> body = statement();

    if (increment != nullptr) 
    {
      body = std::make_shared<Block>(
          std::vector<std::shared_ptr<Stmt>>{
              body,
              std::make_shared<Expression>(increment)});
    }
    
    if (condition == nullptr)
      condition = std::make_shared<Literal>(true);

    body = std::make_shared<While>(condition, body);

    if (initializer != nullptr)
    {
      body = std::make_shared<Block>(
          std::vector<std::shared_ptr<Stmt>>{initializer, body});
    }

    return body;
}

std::shared_ptr<Stmt> Parser::if_statement()
{
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition."); 

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if(match(ELSE))
        elseBranch = statement();

    return std::make_shared<If>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::while_statement()
{
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    std::shared_ptr<Stmt> body = statement();

    return std::make_shared<While>(condition, body);
}

std::shared_ptr<Stmt> Parser::print_statement()
{
    std::shared_ptr<Expr> value = expression();

    consume(SEMICOLON, "Expect ';' after value.");

    return std::make_shared<Print>(value);
}

std::shared_ptr<Stmt> Parser::return_statement() {
    Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (!check(SEMICOLON)) {
      value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<Return>(keyword, value);
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

std::shared_ptr<Function> Parser::function(std::string kind)
{
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(RIGHT_PAREN)) {
      do {
        if (parameters.size() >= 255) {
          error(peek(), "Can't have more than 255 parameters.");
        }

        parameters.emplace_back(
            consume(IDENTIFIER, "Expect parameter name."));
      } while (match(COMMA));
    }
    consume(RIGHT_PAREN, "Expect ')' after parameters.");
    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::shared_ptr<Stmt>> body = block();

    return std::make_shared<Function>(name, parameters, body);
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
    std::shared_ptr<Expr> expr = Or();

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

std::shared_ptr<Expr> Parser::Or()
{
    std::shared_ptr<Expr> expr = And();
    while(match(OR))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = And();
        expr = std::make_shared<Logical>(expr, op, right);
    }
    
    return expr;
}

std::shared_ptr<Expr> Parser::And()
{
    std::shared_ptr<Expr> expr = equality();
    while(match(AND))
    {
        Token op = previous();
        std::shared_ptr<Expr> right = equality();
        expr = std::make_shared<Logical>(expr, op, right);
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
        if(match(FUN)) return function("function");
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

    return call();
}

std::shared_ptr<Expr> Parser::finish_call(std::shared_ptr<Expr> callee)
{
    std::vector<std::shared_ptr<Expr>> arguments;
    if(!check(RIGHT_PAREN))
    {
        do
        {
            if(arguments.size() >= 255)
                error(peek(), "Can't have more than 255 arguments.");

            arguments.emplace_back(expression());
        } while(match(COMMA));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<Call>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::call()
{
    std::shared_ptr<Expr> expr = primary();

    while(true)
    {
        if(match(LEFT_PAREN))
            expr = finish_call(expr);
        else
            break;
    }

    return expr;
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

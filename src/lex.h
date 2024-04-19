#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <any>
#include <map>

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  END
};

class Token
{
public:
    TokenType m_type;
    std::string m_lexeme;
    std::any m_literal;
    int m_line;
    
    Token(TokenType type, std::string lexeme, std::any literal, int line)
        : m_type{type}, m_lexeme{lexeme}, m_literal{literal}, m_line{line}
    {}

    ~Token() = default;

    int get_type(){ return m_type; };
    std::string get_lexeme(){ return m_lexeme; };

    std::string toString() const {
    std::string literal_text;

    switch (m_type) {
      case (IDENTIFIER):
        literal_text = m_lexeme;
        break;
      case (STRING):
        literal_text = std::any_cast<std::string>(m_literal);
        break;
      case (NUMBER):
        literal_text = std::to_string(std::any_cast<double>(m_literal));
        break;
      case (TRUE):
        literal_text = "true";
        break;
      case (FALSE):
        literal_text = "false";
        break;
      default:
        literal_text = "nil";
    }

    return std::to_string(m_type) + " " + m_lexeme + " " + literal_text;
  }
};

class Lexer
{
private:
    std::string m_source;
    std::vector<Token> m_tokens;
    int m_start = 0;
    int m_current = 0;
    int m_line = 1;

    bool is_at_end() { return m_current >= m_source.length(); };
    char advance();
    void add_token(TokenType type);
    void add_token(TokenType type, std::any literal);
    void scan_token();
    bool match(char expected);
    char peek();
    char peek_next();

    void string();
    void number();
    void identifier();

    static std::map<std::string, TokenType> keywords;

public:
    Lexer(std::string source)
        : m_source{source}
    { }
    ~Lexer() = default;

    const std::vector<Token>& scan_tokens();
};
#include "lex.h"
#include <cctype>

void error(int line, std::string message);

std::map<std::string, TokenType> Lexer::keywords =
{
    {"and",     AND},
    {"class",   CLASS},
    {"else",    ELSE},
    {"false",   FALSE},
    {"for",     FOR},
    {"fun",     FUN},
    {"if",      IF},
    {"nil",     NIL},
    {"or",      OR},
    {"print",   PRINT},
    {"return",  RETURN},
    {"super",   SUPER},
    {"this",    THIS},
    {"true",    TRUE},
    {"var",     VAR},
    {"while",   WHILE},
};

const std::vector<Token>& Lexer::scan_tokens()
{
    while (!is_at_end()) {
      this->m_start = this->m_current;
      scan_token();
    }

    this->m_tokens.emplace_back(END, "", "", this->m_line);
    return this->m_tokens;
}

void Lexer::scan_token()
{
    char c = advance();
    switch (c) {
        case '(': this->add_token(LEFT_PAREN); break;
        case ')': this->add_token(RIGHT_PAREN); break;
        case '{': this->add_token(LEFT_BRACE); break;
        case '}': this->add_token(RIGHT_BRACE); break;
        case ',': this->add_token(COMMA); break;
        case '.': this->add_token(DOT); break;
        case '-': this->add_token(MINUS); break;
        case '+': this->add_token(PLUS); break;
        case ';': this->add_token(SEMICOLON); break;
        case '*': this->add_token(STAR); break; 
        case '!':
          this->add_token(match('=') ? BANG_EQUAL : BANG);
          break;
        case '=':
          this->add_token(match('=') ? EQUAL_EQUAL : EQUAL);
          break;
        case '<':
          this->add_token(match('=') ? LESS_EQUAL : LESS);
          break;
        case '>':
          this->add_token(match('=') ? GREATER_EQUAL : GREATER);
          break;  
        case '/':
            if (this->match('/'))
              while (this->peek() != '\n' && !this->is_at_end()) 
                  this->advance();
            else
              this->add_token(SLASH);
          break;
        case ' ':
        case '\r':
        case '\t':
          break;
        case '\n':
          this->m_line++;
          break;
        case '"': this->string(); break;
        case 'o':
            if (match('r'))
              add_token(OR);
  break;
        default:
            if (isdigit(c))
                number();
            else if (isalpha(c))
                identifier();
            else
                error(this->m_line, "Unexpected character.");
        break;
    }
}

char Lexer::advance()
{ 
    return this->m_source.at(this->m_current++);
}

void Lexer::add_token(TokenType type) 
{ 
    this->add_token(type, ""); 
}

void Lexer::add_token(TokenType type, std::any literal)
{
    std::string text = this->m_source.substr(this->m_start, this->m_current - this->m_start);

    this->m_tokens.emplace_back(type, text, literal, this->m_line);
}

bool Lexer::match(char expected)
{
    if (is_at_end()) return false;
    if (this->m_source.at(this->m_current) != expected) return false;

    this->m_current++;
    return true;
}

char Lexer::peek() 
{
    if (this->is_at_end()) return '\0';
    return this->m_source.at(this->m_current);
}

void Lexer::string()
{
    while (this->peek() != '"' && !this->is_at_end()) {
      if (this->peek() == '\n') 
        this->m_line++;
      
      this->advance();
    }

    if (this->is_at_end()) {
      error(this->m_line, "unterminated string.");
      return;
    }

    this->advance();

    std::string value = this->m_source.substr(this->m_start + 1, this->m_current - this->m_start -1);
    this->add_token(STRING, value);
}

void Lexer::number() 
{
    while (isdigit(this->peek())) 
        this->advance();

    if (this->peek() == '.' && isdigit(this->peek_next())) {
      this->advance();

      while(isdigit(peek())) 
        this->advance();
    }

    add_token(NUMBER, stod(m_source.substr(this->m_start, this->m_current - this->m_start)));
}

char Lexer::peek_next()
{
    if (this->m_current + 1 >= this->m_source.length()) 
        return '\0';

    return this->m_source.at(this->m_current + 1);
} 

void Lexer::identifier()
{
    while (isalnum(this->peek())) 
        this->advance();

    std::string text = this->m_source.substr(this->m_start, this->m_current - this->m_start);

    TokenType type;
    if(Lexer::keywords.find(text) == Lexer::keywords.end())
        type = IDENTIFIER;
    else
        type = Lexer::keywords[text];

    add_token(type);
}

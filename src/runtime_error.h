#pragma once

#include <stdexcept>
#include "lex.h"

class RuntimeError : public std::runtime_error 
{
    
public:
    Token m_token;
    
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), m_token(token) {}
};
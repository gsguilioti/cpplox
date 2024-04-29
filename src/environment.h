#pragma once

#include <map>
#include <any>
#include <string>
#include <utility>

#include "lex.h"
#include "runtime_error.h"

class Environment
{
private:
    std::map<std::string, std::any> m_values;

public:
    std::shared_ptr<Environment> m_enclosing;

    Environment()
        : m_enclosing(nullptr) { };
    Environment(std::shared_ptr<Environment> enclosing)
        : m_enclosing(std::move(enclosing)) { };
    ~Environment() = default;

    void define(std::string name, std::any value)
    {
        m_values.emplace(name, value);
    }

    std::any get(Token name)
    {
        if(m_values.count(name.m_lexeme))
            return m_values[name.m_lexeme];

        if(m_enclosing != nullptr) return m_enclosing->get(name);

        throw RuntimeError(name, "Undefined variable '" + name.m_lexeme + "'.");
    }

    void assign(Token name, std::any value)
    {
        if(m_values.count(name.m_lexeme))
        {
            m_values[name.m_lexeme] = value;
            return;
        }

        if(m_enclosing != nullptr)
        {
            m_enclosing->assign(name, value);
            return;
        }
            
        throw RuntimeError(name, "Undefined variable '" + name.m_lexeme + "'.");
    }
};

#include <any>
#include <algorithm>

#include "interpreter.h"
#include "runtime_error.h"

void runtime_error(RuntimeError error);

bool Interpreter::is_truthy(std::any object)
{
    if(object.has_value()) return false;
    if(object.type() == typeid(bool)) return std::any_cast<bool>(object);

    return true;
}

bool Interpreter::is_equal(std::any a, std::any b)
{
    if(!a.has_value() && !b.has_value()) return true;
    if(a.has_value() != b.has_value()) return false;

    if (a.type() != b.type()) return false;
    if (a.type() == typeid(double))
        return std::any_cast<double>(a) == std::any_cast<double>(b);
    else if (a.type() == typeid(std::string)) 
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);

    return false;
}

void Interpreter::check_number_operand(Token op, std::any operand)
{
    if(operand.type() == typeid(double)) return;

    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::check_number_operands(Token op, std::any left, std::any right)
{
    if((left.type() == typeid(double)) && (right.type() == typeid(double))) return;

    throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(std::any object)
{
    if(!object.has_value()) return "nil";

    std::string text;
    if (object.type() == typeid(double))
    {
        text = std::to_string(std::any_cast<double>(object));
        size_t dotPos = text.find('.');
        if (dotPos != std::string::npos) 
        {
            size_t lastNonZeroPos = text.size() - 1;
            while (text[lastNonZeroPos] == '0' && lastNonZeroPos > dotPos)
                lastNonZeroPos--;

            if (text[lastNonZeroPos] == '.') 
                text.erase(lastNonZeroPos, std::string::npos);
            else
                text.erase(lastNonZeroPos + 1, std::string::npos);
        }

        return text;
    } else if (object.type() == typeid(std::string))
    {
        text = std::any_cast<std::string>(object);
    }

    return text;
}

std::any Interpreter::visit_unary(std::shared_ptr<Unary> expr)
{
    std::any right = evaluate(expr->m_right);

    switch(expr->m_operator.m_type)
    {
        case BANG:
            return !is_truthy(right);
        case MINUS:
            check_number_operand(expr->m_operator, right);
            return -(std::any_cast<double>(right));
    }

    return std::any();
}

std::any Interpreter::visit_binary(std::shared_ptr<Binary> expr)
{
    std::any right = evaluate(expr->m_right);
    std::any left = evaluate(expr->m_left);

    switch(expr->m_operator.m_type)
    {
        case GREATER:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case GREATER_EQUAL:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case LESS:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case LESS_EQUAL:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case BANG_EQUAL: 
            check_number_operands(expr->m_operator, left, right);
            return !is_equal(left, right);
        case EQUAL_EQUAL: 
            check_number_operands(expr->m_operator, left, right);
            return is_equal(left, right);
        case MINUS:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case PLUS:
            if(left.type() == typeid(double) && left.type() == typeid(double))
                return std::any_cast<double>(left) + std::any_cast<double>(right);

            if(left.type() == typeid(std::string) && left.type() == typeid(std::string))
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);

            throw RuntimeError(expr->m_operator, "Operands must be two number or two strings");

        case SLASH:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case STAR:
            check_number_operands(expr->m_operator, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
    }

    return std::any();
}

void Interpreter::interpret(std::shared_ptr<Expr> expression)
{
    try
    {
        std::any value = evaluate(expression);
        std::cout << stringify(value);
    }
    catch(RuntimeError error)
    {
        runtime_error(error);
    }
    
}
#include <any>
#include <algorithm>

#include "interpreter.h"
#include "runtime_error.h"
#include "environment.h"
#include "lox_callable.h"
#include "lox_function.h"
#include "lox_return.h"

void runtime_error(RuntimeError error);

bool Interpreter::is_truthy(std::any object)
{
    if (object.type() == typeid(nullptr)) return false;
    if (object.type() == typeid(bool)) {
      return std::any_cast<bool>(object);
    }
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
    else if (object.type() == typeid(bool)) 
    {
      return std::any_cast<bool>(object) ? "true" : "false";
    }
    else if (object.type() == typeid(std::shared_ptr<LoxFunction>)) 
    {
      return std::any_cast<
          std::shared_ptr<LoxFunction>>(object)->to_string();
    }

    return text;
}

std::any Interpreter::visit_logical(std::shared_ptr<Logical> expr)
{
    std::any left = evaluate(expr->m_left);

    if(expr->m_operator.m_type == TokenType::OR)
    {
        if(is_truthy(left)) return left;
    }
    else
    {
        if(!is_truthy(left)) return left;
    }

    return evaluate(expr->m_right);
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

std::any Interpreter::visit_variable(std::shared_ptr<Variable> expr)
{
    return m_environment->get(expr->m_name);
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

std::any Interpreter::visit_call(std::shared_ptr<Call> expr)
{
    std::any callee = evaluate(expr->m_calee);

    std::vector<std::any> arguments;
    for (const std::shared_ptr<Expr>& argument : expr->m_arguments) 
    {
      arguments.push_back(evaluate(argument));
    }

    std::shared_ptr<LoxCallable> function;

    if (callee.type() == typeid(std::shared_ptr<LoxFunction>)) 
      function = std::any_cast<std::shared_ptr<LoxFunction>>(callee);
    else 
      throw RuntimeError{expr->m_paren, "Can only call functions and classes."};

    if (arguments.size() != function->arity()) 
    {
      throw RuntimeError{expr->m_paren, "Expected " +
          std::to_string(function->arity()) + " arguments but got " +
          std::to_string(arguments.size()) + "."};
    }

    return function->call(*this, std::move(arguments));
}

std::any Interpreter::visit_expression(std::shared_ptr<Expression> stmt)
{
    evaluate(stmt->m_expression);

    return std::any();
}

std::any Interpreter::visit_function(std::shared_ptr<Function> stmt)
{
    auto function = std::make_shared<LoxFunction>(stmt, m_environment);
    m_environment->define(stmt->m_name.m_lexeme, function);

    return std::any();
}

std::any Interpreter::visit_if(std::shared_ptr<If> stmt)
{
    if(is_truthy(evaluate(stmt->m_condition)))
        execute(stmt->m_thenBranch);
    else if(stmt->m_elseBranch != nullptr)
        execute(stmt->m_elseBranch);

    return std::any();
}

std::any Interpreter::visit_print(std::shared_ptr<Print> stmt)
{
    std::any value = evaluate(stmt->m_expression);
    std::cout << stringify(value) << "\n";
    
    return std::any();
}

std::any Interpreter::visit_return(std::shared_ptr<Return> stmt) 
{
    std::any value = nullptr;
    if (stmt->m_value != nullptr) 
        value = evaluate(stmt->m_value);

    throw LoxReturn{value};
}

std::any Interpreter::visit_var(std::shared_ptr<Var> stmt)
{
    std::any value;
    if(stmt->m_initializer != nullptr)
        value = evaluate(stmt->m_initializer);

    m_environment->define(stmt->m_name.m_lexeme, std::move(value));
    return std::any();
}

std::any Interpreter::visit_while(std::shared_ptr<While> stmt)
{
    while(is_truthy(evaluate(stmt->m_condition)))
        execute(stmt->m_body);

    return std::any();
}

std::any Interpreter::visit_assign(std::shared_ptr<Assign> expr)
{
    std::any value = evaluate(expr->m_value);
    m_environment->assign(expr->m_name, value);
    return value;
}

std::any Interpreter::visit_block(std::shared_ptr<Block> stmt)
{
    execute_block(stmt->m_statements, std::make_shared<Environment>(m_environment));
    return std::any();
}

void Interpreter::execute_block(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment)
{
    std::shared_ptr<Environment> previous = m_environment;
    try
    {
        m_environment = environment;

        for(const std::shared_ptr<Stmt>& statement : statements)
            execute(statement);
    }
    catch(...)
    {
        m_environment = previous;
        throw;
    }

    m_environment = previous;
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements)
{
    try
    {
        for(const std::shared_ptr<Stmt>& statement : statements)
            execute(statement);
    }
    catch(RuntimeError error)
    {
        runtime_error(error);
    }
}

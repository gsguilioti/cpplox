#pragma once

#include <any>
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include "expr.h"
#include "stmt.h"
#include "environment.h"
#include "lox_callable.h"
#include "lox_function.h"
#include "lox_return.h"

class NativeClock: public LoxCallable {
public:
  int arity() override { return 0; }

  std::any call(Interpreter& interpreter,
                std::vector<std::any> arguments) override {
    auto ticks = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>{ticks}.count() / 1000.0;
  }

  std::string to_string() override { return "<native fn>"; }
};

class Interpreter : public VisitorExpr, public VisitorStmt
{
friend class LoxFunction;

public:
    Interpreter() 
    {
        m_globals->define("clock", std::shared_ptr<NativeClock>{});
    };

    ~Interpreter() = default;

    std::any visit_literal(std::shared_ptr<Literal> expr) override
    { return expr->m_value; };

    std::any visit_grouping(std::shared_ptr<Grouping> expr) override
    { return evaluate(expr->m_expression); };

    std::any visit_unary(std::shared_ptr<Unary> expr) override;
    std::any visit_binary(std::shared_ptr<Binary> expr) override;
    std::any visit_assign(std::shared_ptr<Assign> expr) override;
    std::any visit_call(std::shared_ptr<Call> expr) override;
    std::any visit_get(std::shared_ptr<Get> expr) override {};
    std::any visit_logical(std::shared_ptr<Logical> expr) override;
    std::any visit_set(std::shared_ptr<Set> expr) override {};
    std::any visit_super(std::shared_ptr<Super> expr) override {};
    std::any visit_this(std::shared_ptr<This> expr) override {};
    std::any visit_variable(std::shared_ptr<Variable> expr) override;

    std::any visit_block(std::shared_ptr<Block> stmt) override;
    std::any visit_class(std::shared_ptr<Class> stmt) override {};
    std::any visit_expression(std::shared_ptr<Expression> stmt) override;
    std::any visit_function(std::shared_ptr<Function> stmt) override;
    std::any visit_if(std::shared_ptr<If> stmt) override;
    std::any visit_print(std::shared_ptr<Print> stmt) override;
    std::any visit_return(std::shared_ptr<Return> stmt) override;
    std::any visit_var(std::shared_ptr<Var> stmt) override;
    std::any visit_while(std::shared_ptr<While> stmt) override;

    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

    std::shared_ptr<Environment> m_globals{new Environment};

private:
    std::shared_ptr<Environment> m_environment = m_globals;

    std::any evaluate(std::shared_ptr<Expr> expr)
    { return expr->accept(*this); };

    void execute(std::shared_ptr<Stmt> stmt)
    { stmt->accept(*this); };

    bool is_truthy(std::any object);
    bool is_equal(std::any a, std::any b);
    void check_number_operand(Token op, std::any operand);
    void check_number_operands(Token op, std::any left, std::any right);
    std::string stringify(std::any object);

    void execute_block(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
};

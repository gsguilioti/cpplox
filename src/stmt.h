#pragma once

#include <any>
#include <vector>
#include <utility>
#include "lex.h"
#include "expr.h"

class Block;
class Class;
class Expression;
class Function;
class If;
class Print;
class Return;
class Var;
class While;

class VisitorStmt {
public:
    virtual std::any visit_block(std::shared_ptr<Block> stmt) = 0;
    virtual std::any visit_class(std::shared_ptr<Class> stmt) = 0;
    virtual std::any visit_expression(std::shared_ptr<Expression> stmt) = 0;
    virtual std::any visit_function(std::shared_ptr<Function> stmt) = 0;
    virtual std::any visit_if(std::shared_ptr<If> stmt) = 0;
    virtual std::any visit_print(std::shared_ptr<Print> stmt) = 0;
    virtual std::any visit_return(std::shared_ptr<Return> stmt) = 0;
    virtual std::any visit_var(std::shared_ptr<Var> stmt) = 0;
    virtual std::any visit_while(std::shared_ptr<While> stmt) = 0;
};

class Stmt {
public:
    virtual std::any accept(VisitorStmt& visitor) = 0;
};

class Block : public Stmt, public std::enable_shared_from_this<Block> {
public:
    std::vector<std::shared_ptr<Stmt>> m_statements;

    Block(const std::vector<std::shared_ptr<Stmt>>& statements) 
        : m_statements(std::move(statements)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_block(shared_from_this());
    }
};

class Class : public Stmt, public std::enable_shared_from_this<Class> {
public:
    const Token m_name;
    const std::shared_ptr<Variable> m_superclass;
    const std::vector<std::shared_ptr<Function>> m_methods;

    Class(Token name, std::shared_ptr<Variable> superclass, const std::vector<std::shared_ptr<Function>>& methods) 
        : m_name(std::move(name)), m_superclass(std::move(superclass)), m_methods(std::move(methods)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_class(shared_from_this());
    }
};

class Expression : public Stmt, public std::enable_shared_from_this<Expression> {
public:
    const std::shared_ptr<Expr> m_expression;

    Expression(std::shared_ptr<Expr> expression) 
        : m_expression(std::move(expression)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_expression(shared_from_this());
    }
};

class Function : public Stmt, public std::enable_shared_from_this<Function> {
public:
    const Token m_name;
    const std::vector<Token> m_params;
    const std::vector<std::shared_ptr<Stmt>> m_body;

    Function(Token name, const std::vector<Token>& params, const std::vector<std::shared_ptr<Stmt>>& body) 
        : m_name(std::move(name)), m_params(std::move(params)), m_body(std::move(body)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_function(shared_from_this());
    }
};

class If : public Stmt, public std::enable_shared_from_this<If> {
public:
    const std::shared_ptr<Expr> m_condition;
    const std::shared_ptr<Stmt> m_thenBranch;
    const std::shared_ptr<Stmt> m_elseBranch;

    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch) 
        : m_condition(std::move(condition)), m_thenBranch(std::move(thenBranch)), m_elseBranch(std::move(elseBranch)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_if(shared_from_this());
    }
};

class Print : public Stmt, public std::enable_shared_from_this<Print> {
public:
    const std::shared_ptr<Expr> m_expression;

    Print(std::shared_ptr<Expr> expression) 
        : m_expression(std::move(expression)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_print(shared_from_this());
    }
};

class Return : public Stmt, public std::enable_shared_from_this<Return> {
public:
    const Token m_name;
    const std::shared_ptr<Expr> m_value;

    Return(Token name, std::shared_ptr<Expr> value) 
        : m_name(std::move(name)), m_value(std::move(value)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_return(shared_from_this());
    }
};

class Var : public Stmt, public std::enable_shared_from_this<Var> {
public:
    const Token m_name;
    const std::shared_ptr<Expr> m_initializer;

    Var(Token name, std::shared_ptr<Expr> initializer) 
        : m_name(std::move(name)), m_initializer(std::move(initializer)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_var(shared_from_this());
    }
};

class While : public Stmt, public std::enable_shared_from_this<While> {
public:
    const std::shared_ptr<Expr> m_condition;
    const std::shared_ptr<Stmt> m_body;

    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body) 
        : m_condition(std::move(condition)), m_body(std::move(body)) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_while(shared_from_this());
    }
};

#pragma once

#include <any>
#include <vector>
#include <memory>
#include <utility>
#include "lex.h"

class Assign;
class Binary;
class Call;
class Get;
class Grouping;
class Literal;
class Logical;
class Set;
class Super;
class This;
class Unary;
class Variable;

class VisitorExpr {
public:
    virtual std::any visit_assign(std::shared_ptr<Assign> expr) = 0;
    virtual std::any visit_binary(std::shared_ptr<Binary> expr) = 0;
    virtual std::any visit_call(std::shared_ptr<Call> expr) = 0;
    virtual std::any visit_get(std::shared_ptr<Get> expr) = 0;
    virtual std::any visit_grouping(std::shared_ptr<Grouping> expr) = 0;
    virtual std::any visit_literal(std::shared_ptr<Literal> expr) = 0;
    virtual std::any visit_logical(std::shared_ptr<Logical> expr) = 0;
    virtual std::any visit_set(std::shared_ptr<Set> expr) = 0;
    virtual std::any visit_super(std::shared_ptr<Super> expr) = 0;
    virtual std::any visit_this(std::shared_ptr<This> expr) = 0;
    virtual std::any visit_unary(std::shared_ptr<Unary> expr) = 0;
    virtual std::any visit_variable(std::shared_ptr<Variable> expr) = 0;
};

class Expr {
public:
    virtual std::any accept(VisitorExpr& visitor) = 0;
};

class Assign : Expr, public std::enable_shared_from_this<Assign> {
public:
    const Token m_name;
    std::shared_ptr<Expr> m_value;

    Assign(Token name, std::shared_ptr<Expr> value) 
        : m_name(std::move(name)), m_value(std::move(value)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_assign(shared_from_this());
    }
};

class Binary : Expr, public std::enable_shared_from_this<Binary> {
public:
    std::shared_ptr<Expr> m_left;
    std::shared_ptr<Expr> m_right;
    const Token m_operator;

    Binary(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) 
        : m_operator(op), m_left(std::move(left)), m_right(std::move(right)) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_binary(shared_from_this());
    }
};

class Call : Expr, public std::enable_shared_from_this<Call> {
public:
    const Token m_paren;
    std::shared_ptr<Expr> m_calee;
    const std::vector<std::shared_ptr<Expr>> m_arguments;

    Call(std::shared_ptr<Expr> calee, Token paren, const std::vector<std::shared_ptr<Expr>>& arguments) 
        : m_calee(std::move(calee)), m_paren(std::move(paren)), m_arguments(std::move(arguments)) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_call(shared_from_this());
    }
};

class Get : Expr, public std::enable_shared_from_this<Get> {
public:
    const Token m_name;
    std::shared_ptr<Expr> m_object;

    Get(Token name, std::shared_ptr<Expr> object) 
        : m_name(std::move(name)), m_object(std::move(object)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_get(shared_from_this());
    }
};

class Grouping : Expr, public std::enable_shared_from_this<Grouping> {
public:
    std::shared_ptr<Expr> m_expression;

    Grouping(std::shared_ptr<Expr> expression) 
        : m_expression(std::move(expression)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_grouping(shared_from_this());
    }
};

class Literal : Expr, public std::enable_shared_from_this<Literal> {
public:
    const std::any m_value;

    Literal(std::any value) 
        : m_value(std::move(value)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_literal(shared_from_this());
    }
};

class Logical : Expr, public std::enable_shared_from_this<Logical> {
public:
    std::shared_ptr<Expr> m_left;
    std::shared_ptr<Expr> m_right;
    const Token m_operator;

    Logical(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) 
        : m_operator(std::move(op)), m_left(std::move(left)), m_right(std::move(right)) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_logical(shared_from_this());
    }
};

class Set : Expr, public std::enable_shared_from_this<Set> {
public:
    const Token m_name;
    std::shared_ptr<Expr> m_object;
    std::shared_ptr<Expr> m_value;

    Set(Token name, std::shared_ptr<Expr> value,std::shared_ptr<Expr> object) 
        : m_name(std::move(name)), m_value(std::move(value)), m_object(std::move(object)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_set(shared_from_this());
    }
};

class Super : Expr, public std::enable_shared_from_this<Super> {
public:
    const Token m_keyword;
    const Token m_method;

    Super(Token keyword, Token method) 
        : m_keyword(std::move(keyword)), m_method(std::move(method)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_super(shared_from_this());
    }
};

class This : Expr, public std::enable_shared_from_this<This> {
public:
    const Token m_keyword;

    This(Token keyword) 
        : m_keyword(std::move(keyword)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_this(shared_from_this());
    }
};

class Unary : Expr, public std::enable_shared_from_this<Unary> {
public:
    const Token m_operator;
    std::shared_ptr<Expr> m_right;

    Unary(Token op, std::shared_ptr<Expr> right) 
        : m_operator(std::move(op)), m_right(std::move(right)) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_unary(shared_from_this());
    }
};

class Variable : Expr, public std::enable_shared_from_this<Variable> {
public:
    const Token m_name;

    Variable(Token name) 
        : m_name(std::move(name)) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_variable(shared_from_this());
    }
};

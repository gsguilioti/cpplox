#pragma once

#include <any>
#include <vector>
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
    virtual std::any visit_assign(const Assign& expr) = 0;
    virtual std::any visit_binary(const Binary& expr) = 0;
    virtual std::any visit_call(const Call& expr) = 0;
    virtual std::any visit_get(const Get& expr) = 0;
    virtual std::any visit_grouping(const Grouping& expr) = 0;
    virtual std::any visit_literal(const Literal& expr) = 0;
    virtual std::any visit_logical(const Logical& expr) = 0;
    virtual std::any visit_set(const Set& expr) = 0;
    virtual std::any visit_super(const Super& expr) = 0;
    virtual std::any visit_this(const This& expr) = 0;
    virtual std::any visit_unary(const Unary& expr) = 0;
    virtual std::any visit_variable(const Variable& expr) = 0;
};

class Expr {
public:
    virtual std::any accept(VisitorExpr& visitor) = 0;
};

class Assign : public Expr {
public:
    const Token m_name;
    const Expr& m_value;

    Assign(Token name, Expr& value) 
        : m_name(name), m_value(value) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_assign(*this);
    }
};

class Binary : public Expr {
public:
    const Expr& m_left;
    const Expr& m_right;
    const Token m_operator;

    Binary(Token op, Expr& left, Expr& right) 
        : m_operator(op), m_left(left), m_right(right) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_binary(*this);
    }
};

class Call : public Expr {
public:
    const Token m_paren;
    const Expr& m_calee;
    const std::vector<Expr*> m_arguments;

    Call(Expr& calee, Token paren, const std::vector<Expr*>& arguments) 
        : m_calee(calee), m_paren(paren), m_arguments(arguments) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_call(*this);
    }
};

class Get : public Expr {
public:
    const Token m_name;
    const Expr& m_object;

    Get(Token name, Expr& object) 
        : m_name(name), m_object(object) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_get(*this);
    }
};

class Grouping : public Expr {
public:
    const Expr& m_expression;

    Grouping(Expr& expression) 
        : m_expression(expression) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_grouping(*this);
    }
};

class Literal : public Expr {
public:
    const std::any m_value;

    Literal(std::any value) 
        : m_value(value) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_literal(*this);
    }
};

class Logical : public Expr {
public:
    const Expr& m_left;
    const Expr& m_right;
    const Token m_operator;

    Logical(Token op, Expr& left, Expr& right) 
        : m_operator(op), m_left(left), m_right(right) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_logical(*this);
    }
};

class Set : public Expr {
public:
    const Token m_name;
    const Expr& m_object;
    const Expr& m_value;

    Set(Token name, Expr& value,Expr& object) 
        : m_name(name), m_value(value), m_object(object) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_set(*this);
    }
};

class Super : public Expr {
public:
    const Token m_keyword;
    const Token m_method;

    Super(Token keyword, Token method) 
        : m_keyword(keyword), m_method(method) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_super(*this);
    }
};

class This : public Expr {
public:
    const Token m_keyword;

    This(Token keyword) 
        : m_keyword(keyword) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_this(*this);
    }
};

class Unary : public Expr {
public:
    const Token m_operator;
    const Expr& m_right;

    Unary(Token op, Expr& right) 
        : m_operator(op), m_right(right) {}
    
    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_unary(*this);
    }
};

class Variable : public Expr {
public:
    const Token m_name;

    Variable(Token name) 
        : m_name(name) {}

    virtual std::any accept(VisitorExpr& visitor) override {
        return visitor.visit_variable(*this);
    }
};

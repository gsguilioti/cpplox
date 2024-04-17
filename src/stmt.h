#pragma once

#include <any>
#include <vector>
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
    virtual std::any visit_block(const Block& expr) = 0;
    virtual std::any visit_class(const Class& expr) = 0;
    virtual std::any visit_expression(const Expression& expr) = 0;
    virtual std::any visit_function(const Function& expr) = 0;
    virtual std::any visit_if(const If& expr) = 0;
    virtual std::any visit_print(const Print& expr) = 0;
    virtual std::any visit_return(const Return& expr) = 0;
    virtual std::any visit_var(const Var& expr) = 0;
    virtual std::any visit_while(const While& expr) = 0;
};

class Stmt {
public:
    virtual std::any accept(VisitorStmt& visitor) = 0;
};

class Block : public Stmt {
public:
    const std::vector<Stmt*> m_statements;

    Block(const std::vector<Stmt*>& statements) 
        : m_statements(statements) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_block(*this);
    }
};

class Class : public Stmt {
public:
    const Token m_name;
    const Variable& m_superclass;
    const std::vector<Function*> m_methods;

    Class(Token name, Variable& superclass, const std::vector<Function*>& methods) 
        : m_name(name), m_superclass(superclass), m_methods(methods) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_class(*this);
    }
};

class Expression : public Stmt {
public:
    const Expr& m_expression;

    Expression(Expr& expression) 
        : m_expression(expression) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_expression(*this);
    }
};

class Function : public Stmt {
public:
    const Token m_name;
    const std::vector<Token*> m_params;
    const std::vector<Stmt*> m_body;

    Function(Token name, const std::vector<Token*>& params, const std::vector<Stmt*>& body) 
        : m_name(name), m_params(params), m_body(body) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_function(*this);
    }
};

class If : public Stmt {
public:
    const Expr& m_condition;
    const Stmt& m_thenBranch;
    const Stmt& m_elseBranch;

    If(Expr& condition, Stmt& thenBranch, Stmt& elseBranch) 
        : m_condition(condition), m_thenBranch(thenBranch), m_elseBranch(elseBranch) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_if(*this);
    }
};

class Print : public Stmt {
public:
    const Expr& m_expression;

    Print(Expr& expression) 
        : m_expression(expression) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_print(*this);
    }
};

class Return : public Stmt {
public:
    const Token m_name;
    const Expr& m_value;

    Return(Token name, Expr& value) 
        : m_name(name), m_value(value) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_return(*this);
    }
};

class Var : public Stmt {
public:
    const Token m_name;
    const Expr& m_initializer;

    Var(Token name, Expr& initializer) 
        : m_name(name), m_initializer(initializer) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_var(*this);
    }
};

class While : public Stmt {
public:
    const Expr& m_condition;
    const Stmt& m_body;

    While(Expr& condition, Stmt& body) 
        : m_condition(condition), m_body(body) {}

    virtual std::any accept(VisitorStmt& visitor) override {
        return visitor.visit_while(*this);
    }
};

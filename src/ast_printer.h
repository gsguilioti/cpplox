#pragma once

#include <string>
#include <sstream>
#include <vector>
#include "expr.h"

class AstPrinter : public VisitorExpr
{
public:
    std::any print(std::shared_ptr<Expr> expr){ return std::any_cast<std::string>(expr->accept(*this)); };

    std::any visit_binary(std::shared_ptr<Binary> expr) 
    {
      return parenthesize(expr->m_operator.m_lexeme, expr->m_left, expr->m_right);
    }

    std::any visit_grouping(std::shared_ptr<Grouping> expr) 
    {
      return parenthesize("group", expr->m_expression);
    }

    std::any visit_literal(std::shared_ptr<Literal> expr) 
    {
      auto& value_type = expr->m_value.type();

      if (value_type == typeid(nullptr)) {
        return "nil";
      } else if (value_type == typeid(std::string)) {
        return std::any_cast<std::string>(expr->m_value);
      } else if (value_type == typeid(double)) {
        return std::to_string(std::any_cast<double>(expr->m_value));
      } else if (value_type == typeid(bool)) {
        return std::any_cast<bool>(expr->m_value) ? "true" : "false";
      }
    }

    std::any visit_unary(std::shared_ptr<Unary> expr) 
    {
      return parenthesize(expr->m_operator.m_lexeme, expr->m_right);
    }

private:
    template <class... E>
    std::string parenthesize(const std::string& name, E... expr) {
        std::stringstream builder;
        builder << "(" << name;
        ((builder << " " << std::any_cast<std::string>(expr)), ...);
        builder << ")";

        return builder.str();
    }
};
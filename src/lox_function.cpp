#include "lox_function.h"
#include <utility>        
#include "environment.h"
#include "interpreter.h"
#include "stmt.h"
#include "lox_return.h"

LoxFunction::LoxFunction(std::shared_ptr<Function> declaration,
                         std::shared_ptr<Environment> closure)
  : closure{std::move(closure)}, declaration{std::move(declaration)}
{}

std::string LoxFunction::to_string() {
  return "<fn " + declaration->m_name.m_lexeme + ">";
}

int LoxFunction::arity() {
  return declaration->m_params.size();
}

std::any LoxFunction::call(Interpreter& interpreter,
                           std::vector<std::any> arguments) 
{
  auto environment = std::make_shared<Environment>(closure);
  for (int i = 0; i < declaration->m_params.size(); ++i) {
    environment->define(declaration->m_params[i].m_lexeme,
        arguments[i]);
  }

  try {
    interpreter.execute_block(declaration->m_body, environment);
  } catch (LoxReturn returnValue) {
    return returnValue.value;
  }

  return nullptr;
}
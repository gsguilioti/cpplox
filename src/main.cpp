#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "lex.h"
#include "expr.h"
#include "stmt.h"
#include "ast_printer.h"
#include "parser.h"
#include "runtime_error.h"
#include "interpreter.h"

static Interpreter* interpreter = new Interpreter();

bool had_error = false;
bool had_runtime_error = false;

static void run(std::string source)
{
    Lexer* lexer = new Lexer(source);
    auto tokens = lexer->scan_tokens();
    Parser* parser = new Parser(tokens);
    std::shared_ptr<Expr> expression = parser->parse();

    if (had_error) return;

    std::cout << "\ntokens:\n";
    for (auto& token : tokens)
        std::cout << "[" << token.get_type() << "]" << " token: " << token.get_lexeme() << "\n";

    std::cout << "\nparse:\n";
    std::cout << AstPrinter{}.print(expression) << "\n";

    std::cout << "\ninterpret:\n";
    interpreter->interpret(expression);
    std::cout << "\n";
}

static void report(int line, std::string where, std::string message)
{
    std::cout << "[line " + std::to_string(line) + "] Error" + where + ": " + message;
    had_error = true;
}

extern void error(Token token, std::string message)
{
    if(token.m_type == TokenType::END)
        report(token.m_line, " at end", message);
    else
        report(token.m_line, " at '" + token.m_lexeme + "'", message);
}

extern void runtime_error(RuntimeError error)
{
    std::cout << error.what() << "\n[line " << error.m_token.m_line << "]";
    had_runtime_error = true;
}

static void run_file(std::string filename)
{
    std::ifstream file("../../example/" + filename);
    std::stringstream strStream;

    strStream << file.rdbuf();
    run(strStream.str());

    if (had_error) exit(1);
    if (had_runtime_error) exit(1);
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        run_file(argv[1]);
    }

    return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "lex.h"
#include "expr.h"
#include "stmt.h"
#include "ast_printer.h"

bool had_error = false;

static void run(std::string source)
{
    Lexer* lexer = new Lexer(source);
    auto tokens = lexer->scan_tokens();

    for (auto& token : tokens)
        std::cout << "[" << token.get_type() << "]" << " token: " << token.get_lexeme() << "\n";
}

static void report(int line, std::string where, std::string message)
{
    std::cout << "[line " + std::to_string(line) + "] Error" + where + ": " + message;
    had_error = true;
}

extern void error(int line, std::string message)
{
    report(line, "", message);
}

static void run_file(std::string filename)
{
    std::ifstream file("../../example/" + filename);
    std::stringstream strStream;

    strStream << file.rdbuf();
    run(strStream.str());

    if (had_error) exit(1);
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        run_file(argv[1]);
    }

    return 0;
}

#include <vector>
#include <string>
#include <regex>
#include <string_view>
#include <map>
#include "tokenizer.h"
#include "ast_builder.h"


#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
    fputc((char)X, stderr);
    return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT double printd(double X) {
    fprintf(stderr, "%f\n", X);
    return 0;
}


const std::string source_code = R"(
extern printd(x)
def sum(x) x + x
def main() printd(1111111111.10101)
)";

int main() {
    Tokenizer tokenizer(source_code);
    tokenizer.get_token();
    AST ast;

    ast.build(tokenizer);

    while (true) {
        Tokenizer::Token rr = tokenizer.get_token();
    }

    return 0;
}


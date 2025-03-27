// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "Evaluator/Evaluator.hpp"
#include "Lexing/Lexer.hpp"
#include "Misc/Printer.hpp"
#include "Misc/Stack.hpp"
#include "Parsing/Parser.hpp"
#include "Semantic/Passes/GlobalScopePass.hpp"
#include "Semantic/Passes/LocalScopePass.hpp"
#include "Semantic/Passes/ModuleDefinitionPass.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"

int main(int argc, char* argv[]) {
    setStackBottom();

    std::string input;

    if (argc > 1) {
        std::ifstream inputFile(argv[1]);

        if (!inputFile.is_open()) {
            std::cerr << "Input file couldn't be open";
            return -1;
        }

        input.resize(std::filesystem::file_size(argv[1]));
        inputFile.read(input.data(), static_cast<std::streamsize>(input.size()));
    } else {
        throw std::logic_error("Path to source file not provided");
    }

    auto lex = Lexer(input);

    std::vector<Token> tokens;
    lex.getTokens(tokens);

    for (const auto& item: tokens) {
        std::cout << item.toString() << std::endl;
    }
    auto parse = Parser(lex);

    auto ast = parse.program();

    auto printer = Printer(ast, 2);
    printer.print();

    auto symbolTable = SymbolTable();
    auto context = SymbolContext(symbolTable);

    auto pass1 = ModuleDefinitionPass(*ast, context);
    auto pass2 = GlobalScopePass(*ast, context);
    auto pass3 = LocalScopePass(*ast, context);
    pass1.analyze();
    pass2.analyze();
    pass3.analyze();

    auto start = std::chrono::high_resolution_clock::now();

    auto evaluator = Evaluator(*ast, context);
    evaluator.evaluate();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::wcout << "Run time: " << duration.count() << std::endl;

    system("pause");
    return 0;
}

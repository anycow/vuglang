// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "Codegen/LLVMCodegen.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
#include "Lexing/Lexer.hpp"
#include "Lexing/Token.hpp"
#include "Misc/CLI.hpp"
#include "Misc/Printer.hpp"
#include "Misc/Result.hpp"
#include "Misc/SourceManager.hpp"
#include "Misc/Stack.hpp"
#include "Parsing/Parser.hpp"
#include "Semantic/Passes/GlobalScopePass.hpp"
#include "Semantic/Passes/LocalScopePass.hpp"
#include "Semantic/Passes/ModuleDefinitionPass.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"

int main(int argc, char* argv[]) {
    setStackBottom();

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    CLI cli{};

    std::optional<std::string> inputFile;
    cli.addValue(inputFile).addOption("input");

    std::string outputFile{"a.o"};
    cli.addValue(outputFile).addOption("output");

    llvm::OptimizationLevel optimizationLevel{llvm::OptimizationLevel::O0};
    cli.addValue(optimizationLevel).addOption("opt-level", std::nullopt, [](auto value) {
        using ParseResult = Result<llvm::OptimizationLevel, CLI::ParseError>;
        if (value == "O0") {
            return ParseResult::ok(llvm::OptimizationLevel::O0);
        } else if (value == "O1") {
            return ParseResult::ok(llvm::OptimizationLevel::O1);
        } else if (value == "O2") {
            return ParseResult::ok(llvm::OptimizationLevel::O2);
        } else if (value == "O3") {
            return ParseResult::ok(llvm::OptimizationLevel::O3);
        } else if (value == "Os") {
            return ParseResult::ok(llvm::OptimizationLevel::Os);
        } else if (value == "Oz") {
            return ParseResult::ok(llvm::OptimizationLevel::Oz);
        } else {
            return ParseResult::error(CLI::ParseError::InvalidValue);
        }
    });

    llvm::Triple targetTriple{llvm::sys::getDefaultTargetTriple()};
    cli.addValue(targetTriple).addOption("target", std::nullopt, [](auto value) {
        using ParseResult = Result<llvm::Triple, CLI::ParseError>;
        return ParseResult::ok(llvm::Triple{llvm::Triple::normalize(value)});
    });

    std::string cpu{};
    cli.addValue(cpu).addOption("cpu");

    std::string features{};
    cli.addValue(features).addOption("features");

    llvm::PICLevel::Level picLevel{llvm::PICLevel::NotPIC};
    cli.addValue(picLevel).addOption("pic", llvm::PICLevel::BigPIC, [](auto value) {
        using ParseResult = Result<llvm::PICLevel::Level, CLI::ParseError>;
        if (value == "no") {
            return ParseResult::ok(llvm::PICLevel::NotPIC);
        } else if (value == "small") {
            return ParseResult::ok(llvm::PICLevel::SmallPIC);
        } else if (value == "big") {
            return ParseResult::ok(llvm::PICLevel::BigPIC);
        } else {
            return ParseResult::error(CLI::ParseError::InvalidValue);
        }
    });

    llvm::PIELevel::Level pieLevel{llvm::PIELevel::Default};
    cli.addValue(pieLevel).addOption("pie", llvm::PIELevel::Large, [](auto value) {
        using ParseResult = Result<llvm::PIELevel::Level, CLI::ParseError>;
        if (value == "no" || value == "default") {
            return ParseResult::ok(llvm::PIELevel::Default);
        } else if (value == "small") {
            return ParseResult::ok(llvm::PIELevel::Small);
        } else if (value == "big" || value == "large") {
            return ParseResult::ok(llvm::PIELevel::Large);
        } else {
            return ParseResult::error(CLI::ParseError::InvalidValue);
        }
    });

    llvm::CodeModel::Model codeModel{llvm::CodeModel::Small};
    cli.addValue(codeModel).addOption("code-model", std::nullopt, [](auto value) {
        using ParseResult = Result<llvm::CodeModel::Model, CLI::ParseError>;
        if (value == "tiny") {
            return ParseResult::ok(llvm::CodeModel::Model::Tiny);
        } else if (value == "small") {
            return ParseResult::ok(llvm::CodeModel::Model::Small);
        } else if (value == "kernel") {
            return ParseResult::ok(llvm::CodeModel::Model::Kernel);
        } else if (value == "medium") {
            return ParseResult::ok(llvm::CodeModel::Model::Medium);
        } else if (value == "large") {
            return ParseResult::ok(llvm::CodeModel::Model::Large);
        } else {
            return ParseResult::error(CLI::ParseError::InvalidValue);
        }
    });

    bool verify{false};
    cli.addValue(verify).addOption("verify", true);

    bool noBuiltin{false};
    cli.addValue(noBuiltin).addOption("no-builtin", true);

    cli.parse(argc, argv).unwrap();

    if (pieLevel != llvm::PIELevel::Default) {
        switch (pieLevel) {
            case llvm::PIELevel::Default:
                std::abort();
                break;
            case llvm::PIELevel::Small:
                picLevel = llvm::PICLevel::SmallPIC;
                break;
            case llvm::PIELevel::Large:
                picLevel = llvm::PICLevel::BigPIC;
                break;
        }
    }

    std::string input;

    std::ifstream inputStream{*inputFile};

    if (!inputStream.is_open()) {
        std::cerr << "Input file couldn't be open";
        std::exit(-1);
    }

    input.resize(std::filesystem::file_size(*inputFile));
    inputStream.read(input.data(), static_cast<std::streamsize>(input.size()));

    const SourceFile file{std::filesystem::path(*inputFile).filename().string(), std::move(input)};
    Lexer lex{file};

    std::vector<Token> tokens;
    lex.getTokens(tokens);
    // for (const auto& token : tokens) {
    //     //std::cout << token.toString() << std::endl;
    // }

    DiagnosticManager diagnosticManager{DiagnosticMessage::Severity::Hint};
    auto parse = Parser(lex, diagnosticManager);

    auto ast = parse.program();

    auto printer = Printer(*ast, 2);
    printer.print();

    SymbolTable symbolTable{};
    auto context = SymbolContext(symbolTable);

    ModuleDefinitionPass pass1{*ast, context, diagnosticManager};
    GlobalScopePass pass2{*ast, context, diagnosticManager};
    LocalScopePass pass3{*ast, context, diagnosticManager};
    pass1.analyze();
    pass2.analyze();
    pass3.analyze();

    if (diagnosticManager.error_count() > 0 || diagnosticManager.fatal_count() > 0) {
        return 0;
    }

    LLVMCodegen codegen(*ast,
                        context,
                        {
                            .optimizationLevel = optimizationLevel,
                            .cpu = cpu,
                            .features = features,
                            .picLevel = picLevel,
                            .pieLevel = pieLevel,
                            .codeModel = codeModel,
                            .verify = verify,
                            .hasBuiltin = !noBuiltin,
                        },
                        std::filesystem::path(*inputFile).filename().string(),
                        targetTriple);
    codegen.emit();

    std::error_code errorCode;
    llvm::raw_fd_ostream dest{outputFile, errorCode, llvm::sys::fs::OF_None};
    if (errorCode) {
        std::cerr << "Could not open file: " << errorCode.message() << "\n";
        std::exit(-1);
    }

    llvm::legacy::PassManager emitPass;
    if (codegen.getTargetMachine()->addPassesToEmitFile(emitPass,
                                                        dest,
                                                        nullptr,
                                                        llvm::CodeGenFileType::ObjectFile)) {
        std::cerr << "TargetMachine can't emit a file\n";
        std::exit(-1);
    }

    emitPass.run(codegen.getModule());
    dest.flush();

    codegen.getModule().print(llvm::outs(), nullptr);

    llvm::llvm_shutdown();
    return 0;
}

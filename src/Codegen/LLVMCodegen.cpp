// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LLVMCodegen.hpp"

#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/CodeGen/MachineFunctionAnalysisManager.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>

#include "AST/Nodes/Declarations/ModuleDeclaration.hpp"
#include "AST/Nodes/Node.hpp"
#include "Codegen/LLVMDeclarationCodegen.hpp"
#include "Codegen/LLVMDefinitionCodegen.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/Types/IntegerType.hpp"

std::string LLVMCodegen::emit(const std::string& fileName) {
    stackGuard();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    mModule->setSourceFileName(fileName);

    auto targetTriple = llvm::Triple(llvm::sys::getDefaultTargetTriple());
    std::string error;
    const auto* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        std::cerr << error;
        std::abort();
    }

    const auto* cpu = "generic";
    const auto* features = "";
    const llvm::TargetOptions opt;
    auto* targetMachine
        = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::Static);
    mModule->setTargetTriple(targetTriple);
    mModule->setDataLayout(targetMachine->createDataLayout());

    for (auto& symbol : mSymbolContext.getSignedIntegerTypeSymbols()) {
        mTypes.emplace(
            symbol.second->getType(),
            llvm::IntegerType::get(*mContext,
                                   static_cast<IntegerType*>(symbol.second->getType())->getBits()));
    }
    for (auto& symbol : mSymbolContext.getUnsignedIntegerTypeSymbols()) {
        mTypes.emplace(
            symbol.second->getType(),
            llvm::IntegerType::get(*mContext,
                                   static_cast<IntegerType*>(symbol.second->getType())->getBits()));
    }
    mTypes.emplace(mSymbolContext.getBoolType()->getType(), llvm::IntegerType::get(*mContext, 1));

    if (mAST.kind != Node::Kind::ModuleDeclaration) {
        throw std::logic_error("unreachable");
    }
    LLVMDeclarationCodegen declarationCodegen(*this);
    declarationCodegen.emit(static_cast<const ModuleDeclaration&>(mAST));

    LLVMDefinitionCodegen definitionCodegen(*this);
    definitionCodegen.emit(static_cast<const ModuleDeclaration&>(mAST));

    if (llvm::verifyModule(*mModule, &llvm::errs())) {
        llvm::errs() << "Module verification failed.\n";
        std::abort();
    }

    llvm::MachineFunctionAnalysisManager machineFunctionAnalysisManager;
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cgsccAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;
    llvm::PassBuilder passBuilder(targetMachine);
    passBuilder.registerMachineFunctionAnalyses(machineFunctionAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cgsccAnalysisManager);
    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager,
                                     functionAnalysisManager,
                                     cgsccAnalysisManager,
                                     moduleAnalysisManager,
                                     &machineFunctionAnalysisManager);


    llvm::ModulePassManager modulePassManager
        = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);

    modulePassManager.run(*mModule, moduleAnalysisManager);

    std::error_code errorCode;
    llvm::raw_fd_ostream dest("output.o", errorCode, llvm::sys::fs::OF_None);
    if (errorCode) {
        std::cerr << "Could not open file: " << errorCode.message() << "\n";
        std::abort();
    }

    llvm::legacy::PassManager emitPass;
    if (targetMachine->addPassesToEmitFile(emitPass,
                                           dest,
                                           nullptr,
                                           llvm::CodeGenFileType::ObjectFile)) {
        std::cerr << "TargetMachine can't emit a file\n";
        std::abort();
    }

    emitPass.run(*mModule);
    dest.flush();

    std::string moduleString;
    llvm::raw_string_ostream stream(moduleString);
    mModule->print(stream, nullptr);

    return moduleString;
}

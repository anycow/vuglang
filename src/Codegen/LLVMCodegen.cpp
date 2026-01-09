// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LLVMCodegen.hpp"

#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
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

#include "AST/Nodes/Declarations/ModuleDeclaration.hpp"
#include "AST/Nodes/Node.hpp"
#include "Codegen/LLVMDeclarationCodegen.hpp"
#include "Codegen/LLVMDefinitionCodegen.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/Types/IntegerType.hpp"

void LLVMCodegen::emit() {
    stackGuard();

    mModule->setSourceFileName(mInputName);

    std::string error;
    mTarget = llvm::TargetRegistry::lookupTarget(mTargetTriple, error);
    if (!mTarget) {
        std::cerr << error;
        std::abort();
    }

    const llvm::TargetOptions targetOptions;
    mTargetMachine = mTarget->createTargetMachine(
        mTargetTriple,
        mCodegenOptions.cpu,
        mCodegenOptions.features,
        targetOptions,
        (mCodegenOptions.picLevel == 0) ? llvm::Reloc::Static : llvm::Reloc::PIC_,
        mCodegenOptions.codeModel,
        *llvm::CodeGenOpt::getLevel(
            static_cast<int>(mCodegenOptions.optimizationLevel.getSpeedupLevel())));
    mModule->setTargetTriple(mTargetTriple);
    mModule->setDataLayout(mTargetMachine->createDataLayout());
    mModule->setPICLevel(mCodegenOptions.picLevel);
    mModule->setPIELevel(mCodegenOptions.pieLevel);

    llvm::TargetLibraryInfoImpl targetLibraryInfo(mTargetTriple);
    if (!mCodegenOptions.hasBuiltin) {
        targetLibraryInfo.disableAllFunctions();
    }

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

    if (mCodegenOptions.verify && llvm::verifyModule(*mModule, &llvm::errs())) {
        llvm::errs() << "Module verification failed.\n";
        std::exit(-1);
    }

    llvm::MachineFunctionAnalysisManager machineFunctionAnalysisManager;
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cgsccAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;
    llvm::PassBuilder passBuilder(mTargetMachine);
    functionAnalysisManager.registerPass([&] {
        return llvm::TargetLibraryAnalysis(targetLibraryInfo);
    });
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
        = passBuilder.buildPerModuleDefaultPipeline(mCodegenOptions.optimizationLevel);

    modulePassManager.run(*mModule, moduleAnalysisManager);
}

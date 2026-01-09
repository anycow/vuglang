// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_LLVMCODEGEN_HPP
#define VUG_LLVMCODEGEN_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/TargetParser/Triple.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "AST/ASTNodesForward.hpp"

namespace llvm {
class Target;
class TargetMachine;
}  // namespace llvm
class Symbol;
class FunctionSymbol;
class SymbolContext;
class Type;

struct CodegenOptions {
    const llvm::OptimizationLevel optimizationLevel{llvm::OptimizationLevel::O0};
    const std::string cpu{"generic"};
    const std::string features;
    const llvm::PICLevel::Level picLevel{llvm::PICLevel::NotPIC};
    const llvm::PIELevel::Level pieLevel{llvm::PIELevel::Default};
    const llvm::CodeModel::Model codeModel{llvm::CodeModel::Small};
    const bool verify{false};
    const bool hasBuiltin{false};
};

class LLVMCodegen {
   public:
    LLVMCodegen(const Node& ast,
                const SymbolContext& symbolContext,
                CodegenOptions codegenOptions,
                std::string inputName,
                llvm::Triple targetTriple)
        : mAST(ast),
          mSymbolContext(symbolContext),
          mCodegenOptions(std::move(codegenOptions)),
          mInputName(std::move(inputName)),
          mTargetTriple(std::move(targetTriple)),
          mContext(std::make_unique<llvm::LLVMContext>()),
          mModule(std::make_unique<llvm::Module>(mInputName, *mContext)),
          mBuilder(std::make_unique<llvm::IRBuilder<>>(*mContext)) {
    }

    void emit();

    [[nodiscard]] llvm::LLVMContext& getContext() {
        return *mContext;
    }
    [[nodiscard]] const llvm::LLVMContext& getContext() const {
        return *mContext;
    }

    [[nodiscard]] const llvm::Target* getTarget() const {
        return mTarget;
    }

    [[nodiscard]] llvm::TargetMachine* getTargetMachine() {
        return mTargetMachine;
    }
    [[nodiscard]] const llvm::TargetMachine* getTargetMachine() const {
        return mTargetMachine;
    }

    [[nodiscard]] llvm::Module& getModule() {
        return *mModule;
    }
    [[nodiscard]] const llvm::Module& getModule() const {
        return *mModule;
    }

    [[nodiscard]] llvm::IRBuilder<>& getBuilder() {
        return *mBuilder;
    }
    [[nodiscard]] const llvm::IRBuilder<>& getBuilder() const {
        return *mBuilder;
    }

    [[nodiscard]] std::unordered_map<const Type*, llvm::Type*>& getTypes() {
        return mTypes;
    }
    [[nodiscard]] const std::unordered_map<const Type*, llvm::Type*>& getTypes() const {
        return mTypes;
    }

    [[nodiscard]] std::unordered_map<const Symbol*, llvm::AllocaInst*>& getObjects() {
        return mObjects;
    }
    [[nodiscard]] const std::unordered_map<const Symbol*, llvm::AllocaInst*>& getObjects() const {
        return mObjects;
    }

    [[nodiscard]] std::unordered_map<const FunctionSymbol*, llvm::Function*>& getFunctions() {
        return mFunctions;
    }
    [[nodiscard]] const std::unordered_map<const FunctionSymbol*, llvm::Function*>& getFunctions()
        const {
        return mFunctions;
    }

   private:
    const Node& mAST;
    const SymbolContext& mSymbolContext;
    const CodegenOptions mCodegenOptions;
    const std::string mInputName;
    const llvm::Triple mTargetTriple;

    const llvm::Target* mTarget{};
    llvm::TargetMachine* mTargetMachine{};
    std::unique_ptr<llvm::LLVMContext> mContext;
    std::unique_ptr<llvm::Module> mModule;
    std::unique_ptr<llvm::IRBuilder<>> mBuilder;


    std::unordered_map<const Type*, llvm::Type*> mTypes;
    std::unordered_map<const Symbol*, llvm::AllocaInst*> mObjects;
    std::unordered_map<const FunctionSymbol*, llvm::Function*> mFunctions;
};

#endif

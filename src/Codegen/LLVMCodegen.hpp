// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_LLVMCODEGEN_HPP
#define VUG_LLVMCODEGEN_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>

#include "AST/ASTNodesForward.hpp"

class Symbol;
class FunctionSymbol;
class SymbolContext;
class Type;

class LLVMCodegen {
   public:
    LLVMCodegen(const Node& ast, const SymbolContext& symbolContext)
        : mAST(ast),
          mSymbolContext(symbolContext),
          mContext(std::make_unique<llvm::LLVMContext>()),
          mModule(std::make_unique<llvm::Module>("vug", *mContext)),
          mBuilder(std::make_unique<llvm::IRBuilder<>>(*mContext)) {
    }

    std::string emit(const std::string& fileName);

    [[nodiscard]] llvm::LLVMContext& getContext() {
        return *mContext;
    }
    [[nodiscard]] const llvm::LLVMContext& getContext() const {
        return *mContext;
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

    std::unique_ptr<llvm::LLVMContext> mContext;
    std::unique_ptr<llvm::Module> mModule;
    std::unique_ptr<llvm::IRBuilder<>> mBuilder;

    std::unordered_map<const Type*, llvm::Type*> mTypes;
    std::unordered_map<const Symbol*, llvm::AllocaInst*> mObjects;
    std::unordered_map<const FunctionSymbol*, llvm::Function*> mFunctions;
};
class LLVMDeclarationCodegen;
class LLVMDefinitionCodegen;


#endif

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_LLVMDECLARATIONCODEGEN_HPP
#define VUG_LLVMDECLARATIONCODEGEN_HPP

#include <llvm/IR/LLVMContext.h>
#include <unordered_map>

#include "AST/ASTNodesForward.hpp"
#include "Codegen/LLVMCodegen.hpp"

class FunctionSymbol;
class LLVMCodegen;
class Type;

class LLVMDeclarationCodegen {
   public:
    explicit LLVMDeclarationCodegen(LLVMCodegen& codegen)
        : mCodegen(codegen) {
    }

    void emit(const ModuleDeclaration& node);
    void emit(const Declaration& node);
    void emit(const DeclarationsBlock& node);
    void emit(const FunctionDeclaration& node);

   private:
    [[nodiscard]] std::unordered_map<const Type*, llvm::Type*>& getTypes() {
        return mCodegen.getTypes();
    }
    [[nodiscard]] const std::unordered_map<const Type*, llvm::Type*>& getTypes() const {
        return mCodegen.getTypes();
    }

    [[nodiscard]] llvm::Module& getModule() {
        return mCodegen.getModule();
    }
    [[nodiscard]] const llvm::Module& getModule() const {
        return mCodegen.getModule();
    }

    [[nodiscard]] std::unordered_map<const FunctionSymbol*, llvm::Function*>& getFunctions() {
        return mCodegen.getFunctions();
    }
    [[nodiscard]] const std::unordered_map<const FunctionSymbol*, llvm::Function*>& getFunctions()
        const {
        return mCodegen.getFunctions();
    }

    LLVMCodegen& mCodegen;
};

#endif

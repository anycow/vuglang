// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_LLVMDEFINITIONCODEGEN_HPP
#define VUG_LLVMDEFINITIONCODEGEN_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <unordered_map>

#include "AST/ASTNodesForward.hpp"
#include "Codegen/LLVMCodegen.hpp"

class FunctionSymbol;
class Symbol;
class Type;
class LLVMCodegen;


class LLVMDefinitionCodegen {
   public:
    explicit LLVMDefinitionCodegen(LLVMCodegen& codegen)
        : mCodegen(codegen) {
    }

    void emit(const Node& node);
    llvm::Value* emit(const Expression& node);
    void emit(const Statement& node);

    void emit(const ModuleDeclaration& node);
    void emit(const Declaration& node);
    void emit(const DeclarationsBlock& node);
    void emit(const FunctionDeclaration& node);

    llvm::Value* emit(const BinaryOperation& node);
    llvm::Value* emit(const PrefixOperation& node);
    llvm::Value* emit(const CallFunction& node);
    llvm::Value* emit(const Identifier& node);
    llvm::Value* emit(const Number& node);

    void emit(const Assign& node);
    void emit(const Break& node);
    void emit(const ExpressionStatement& node);
    void emit(const If& node);
    void emit(const LocalVariableDeclaration& node);
    void emit(const Print& node);
    void emit(const Return& node);
    void emit(const StatementsBlock& node);
    void emit(const While& node);

   private:
    [[nodiscard]] llvm::LLVMContext& getContext() {
        return mCodegen.getContext();
    }
    [[nodiscard]] const llvm::LLVMContext& getContext() const {
        return mCodegen.getContext();
    }

    [[nodiscard]] llvm::IRBuilder<>& getBuilder() {
        return mCodegen.getBuilder();
    }
    [[nodiscard]] const llvm::IRBuilder<>& getBuilder() const {
        return mCodegen.getBuilder();
    }

    [[nodiscard]] std::unordered_map<const Type*, llvm::Type*>& getTypes() {
        return mCodegen.getTypes();
    }
    [[nodiscard]] const std::unordered_map<const Type*, llvm::Type*>& getTypes() const {
        return mCodegen.getTypes();
    }

    [[nodiscard]] std::unordered_map<const Symbol*, llvm::AllocaInst*>& getObjects() {
        return mCodegen.getObjects();
    }
    [[nodiscard]] const std::unordered_map<const Symbol*, llvm::AllocaInst*>& getObjects() const {
        return mCodegen.getObjects();
    }

    [[nodiscard]] std::unordered_map<const FunctionSymbol*, llvm::Function*>& getFunctions() {
        return mCodegen.getFunctions();
    }
    [[nodiscard]] const std::unordered_map<const FunctionSymbol*, llvm::Function*>& getFunctions()
        const {
        return mCodegen.getFunctions();
    }

    void setBlock(llvm::BasicBlock* block) {
        auto* function = getBuilder().GetInsertBlock()->getParent();
        getBuilder().SetInsertPoint(block);
        function->insert(function->end(), block);
    }

    LLVMCodegen& mCodegen;
    class CycleInfo {
       public:
        explicit CycleInfo(llvm::BasicBlock* mergeBlock)
            : mMergeBlock(mergeBlock) {
        }

        [[nodiscard]] llvm::BasicBlock* getMergeBlock() const {
            return mMergeBlock;
        }

       private:
        llvm::BasicBlock* mMergeBlock;
    };
    std::stack<CycleInfo> mCycles;
};

#endif

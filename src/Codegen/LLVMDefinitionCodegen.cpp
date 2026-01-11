// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LLVMDefinitionCodegen.hpp"

#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "AST/Declarations.hpp"
#include "AST/Expressions.hpp"
#include "AST/Statements.hpp"
#include "Codegen/LLVMCodegen.hpp"
#include "Lexing/Token.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/Types/IntegerType.hpp"

void LLVMDefinitionCodegen::emit(const Node& node) {
    stackGuard();

    if (node.isDeclaration()) {
        emit(static_cast<const Declaration&>(node));
    } else if (node.isExpression()) {
        emit(static_cast<const Expression&>(node));
    } else if (node.isStatement()) {
        emit(static_cast<const Statement&>(node));
    }

    switch (node.getKind()) {
        case Node::Kind::Assign:
            emit(static_cast<const Assign&>(node));
            break;
        case Node::Kind::BinaryOperation:
            emit(static_cast<const BinaryOperation&>(node));
            break;
        case Node::Kind::Identifier:
            emit(static_cast<const Identifier&>(node));
            break;
        case Node::Kind::Number:
            emit(static_cast<const Number&>(node));
            break;
        case Node::Kind::PrefixOperation:
            emit(static_cast<const PrefixOperation&>(node));
            break;
        case Node::Kind::StatementBlock:
            emit(static_cast<const StatementsBlock&>(node));
            break;
        case Node::Kind::Break:
            emit(static_cast<const Break&>(node));
            break;
        case Node::Kind::CallFunction:
            emit(static_cast<const CallFunction&>(node));
            break;
        case Node::Kind::If:
            emit(static_cast<const If&>(node));
            break;
        case Node::Kind::Return:
            emit(static_cast<const Return&>(node));
            break;
        case Node::Kind::LocalVarDeclaration:
            emit(static_cast<const LocalVariableDeclaration&>(node));
            break;
        case Node::Kind::While:
            emit(static_cast<const While&>(node));
            break;
        default:
            throw std::logic_error("unreachable");
    }
}

void LLVMDefinitionCodegen::emit(const Declaration& node) {
    stackGuard();

    switch (node.getKind()) {
        case Node::Kind::DeclarationsBlock:
            emit(static_cast<const DeclarationsBlock&>(node));
            break;
        case Node::Kind::FunctionDeclaration:
            emit(static_cast<const FunctionDeclaration&>(node));
            break;
        case Node::Kind::ModuleDeclaration:
            emit(static_cast<const ModuleDeclaration&>(node));
            break;
        default:
            throw std::logic_error("unreachable");
    }
}
void LLVMDefinitionCodegen::emit(const ModuleDeclaration& node) {
    stackGuard();

    if (node.getBody()) {
        emit(*node.getBody());
    }
}
void LLVMDefinitionCodegen::emit(const DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.getDeclarations()) {
        if (declaration->getKind() == Node::Kind::FunctionDeclaration) {
            emit(static_cast<const FunctionDeclaration&>(*declaration));
        }
    }
}
void LLVMDefinitionCodegen::emit(const FunctionDeclaration& node) {
    stackGuard();

    if (node.getDefinition()) {
        auto* function{getFunctions()[node.getSymbolRef()]};
        if (!function) {
            throw std::logic_error("function not declared");
        }

        llvm::BasicBlock* body{llvm::BasicBlock::Create(getContext(), "", function)};
        getBuilder().SetInsertPoint(body);

        for (size_t i = 0; i < node.getParameters().size(); ++i) {
            auto* parameter{function->getArg(i)};

            parameter->setName(node.getParameters()[i]->getName().getValue());
            getObjects()[node.getParameters()[i]->getSymbolRef()]
                = getBuilder().CreateAlloca(parameter->getType());
            getObjects()[node.getParameters()[i]->getSymbolRef()];
            getBuilder().CreateStore(parameter,
                                     getObjects()[node.getParameters()[i]->getSymbolRef()]);
        }

        emit(*node.getDefinition());

        auto* lastBlock{getBuilder().GetInsertBlock()};
        if (lastBlock->empty()) {
            if (&function->getEntryBlock() != lastBlock && lastBlock->use_empty()) {
                lastBlock->eraseFromParent();
                lastBlock = nullptr;
            }
        }
        if (lastBlock && !lastBlock->getTerminator()) {
            getBuilder().CreateRet(llvm::PoisonValue::get(function->getReturnType()));
        }
        getBuilder().ClearInsertionPoint();
    }
}

llvm::Value* LLVMDefinitionCodegen::emit(const Expression& node) {
    stackGuard();

    switch (node.getKind()) {
        case Node::Kind::BinaryOperation:
            return emit(static_cast<const BinaryOperation&>(node));
        case Node::Kind::PrefixOperation:
            return emit(static_cast<const PrefixOperation&>(node));
        case Node::Kind::CallFunction:
            return emit(static_cast<const CallFunction&>(node));
        case Node::Kind::Identifier:
            return emit(static_cast<const Identifier&>(node));
        case Node::Kind::Number:
            return emit(static_cast<const Number&>(node));
        default:
            throw std::logic_error("unreachable");
    }
}
llvm::Value* LLVMDefinitionCodegen::emit(const BinaryOperation& node) {
    stackGuard();

    if (node.getLeft()->getExprType()->isInteger() && node.getRight()->getExprType()->isInteger()) {
        auto* left{emit(*node.getLeft())};
        auto* right{emit(*node.getRight())};

        if (node.getLeft()->getExprType()->getInteger().isSigned()) {
            switch (node.getOperationType()) {
                case LexemType::Plus:
                    return getBuilder().CreateAdd(left, right, "", false, true);
                case LexemType::Minus:
                    return getBuilder().CreateSub(left, right, "", false, true);
                case LexemType::Multiply:
                    return getBuilder().CreateMul(left, right, "", false, true);
                case LexemType::Divide:
                    return getBuilder().CreateSDiv(left, right);
                case LexemType::Remainder:
                    return getBuilder().CreateSRem(left, right);
                case LexemType::BitAnd:
                    return getBuilder().CreateAnd(left, right);
                case LexemType::BitOr:
                    return getBuilder().CreateOr(left, right);
                case LexemType::BitXor:
                    return getBuilder().CreateXor(left, right);

                case LexemType::Equal:
                    return getBuilder().CreateICmpEQ(left, right);
                case LexemType::Unequal:
                    return getBuilder().CreateICmpNE(left, right);
                case LexemType::Less:
                    return getBuilder().CreateICmpSLT(left, right);
                case LexemType::LessEqual:
                    return getBuilder().CreateICmpSLE(left, right);
                case LexemType::Greater:
                    return getBuilder().CreateICmpSGT(left, right);
                case LexemType::GreaterEqual:
                    return getBuilder().CreateICmpSGE(left, right);
                default:
                    throw std::logic_error("unreachable");
            }
        } else {
            switch (node.getOperationType()) {
                case LexemType::Plus:
                    return getBuilder().CreateAdd(left, right);
                case LexemType::Minus:
                    return getBuilder().CreateSub(left, right);
                case LexemType::Multiply:
                    return getBuilder().CreateMul(left, right);
                case LexemType::Divide:
                    return getBuilder().CreateUDiv(left, right);
                case LexemType::Remainder:
                    return getBuilder().CreateURem(left, right);
                case LexemType::BitAnd:
                    return getBuilder().CreateAnd(left, right);
                case LexemType::BitOr:
                    return getBuilder().CreateOr(left, right);
                case LexemType::BitXor:
                    return getBuilder().CreateXor(left, right);

                case LexemType::Equal:
                    return getBuilder().CreateICmpEQ(left, right);
                case LexemType::Unequal:
                    return getBuilder().CreateICmpNE(left, right);
                case LexemType::Less:
                    return getBuilder().CreateICmpULT(left, right);
                case LexemType::LessEqual:
                    return getBuilder().CreateICmpULE(left, right);
                case LexemType::Greater:
                    return getBuilder().CreateICmpUGT(left, right);
                case LexemType::GreaterEqual:
                    return getBuilder().CreateICmpUGE(left, right);
                default:
                    throw std::logic_error("unreachable");
            }
        }
    } else if (node.getLeft()->getExprType()->isBoolean()
               && node.getRight()->getExprType()->isBoolean()) {
        if ((node.getOperationType() != LexemType::LogicAnd)
            && (node.getOperationType() != LexemType::LogicOr)) {
            auto* left{emit(*node.getLeft())};
            auto* right{emit(*node.getRight())};

            switch (node.getOperationType()) {
                case LexemType::BitAnd:
                    return getBuilder().CreateAnd(left, right);
                case LexemType::BitOr:
                    return getBuilder().CreateOr(left, right);
                case LexemType::BitXor:
                    return getBuilder().CreateXor(left, right);
                default:
                    throw std::logic_error("unreachable");
            }
        } else {
            auto* left{emit(*node.getLeft())};

            switch (node.getOperationType()) {
                case LexemType::LogicAnd: {
                    auto* leftBlock{llvm::BasicBlock::Create(getContext())};
                    auto* rightBlock{llvm::BasicBlock::Create(getContext())};
                    auto* mergeBlock{llvm::BasicBlock::Create(getContext())};
                    getBuilder().CreateBr(leftBlock);

                    setBlock(leftBlock);
                    getBuilder().CreateCondBr(left, rightBlock, mergeBlock);

                    setBlock(rightBlock);
                    auto* right{emit(*node.getRight())};
                    getBuilder().CreateBr(mergeBlock);

                    setBlock(mergeBlock);
                    auto* result{getBuilder().CreatePHI(getTypes()[node.getExprType()], 2)};
                    result->addIncoming(llvm::ConstantInt::getBool(getContext(), false), leftBlock);
                    result->addIncoming(right, rightBlock);

                    return result;
                }
                case LexemType::LogicOr: {
                    auto* leftBlock{llvm::BasicBlock::Create(getContext())};
                    auto* rightBlock{llvm::BasicBlock::Create(getContext())};
                    auto* mergeBlock{llvm::BasicBlock::Create(getContext())};
                    getBuilder().CreateBr(leftBlock);

                    setBlock(leftBlock);
                    getBuilder().CreateCondBr(left, mergeBlock, rightBlock);

                    setBlock(rightBlock);
                    auto* right{emit(*node.getRight())};
                    getBuilder().CreateBr(mergeBlock);

                    setBlock(mergeBlock);
                    auto* result{getBuilder().CreatePHI(getTypes()[node.getExprType()], 2)};
                    result->addIncoming(llvm::ConstantInt::getBool(getContext(), true), leftBlock);
                    result->addIncoming(right, rightBlock);

                    return result;
                }
                default:
                    throw std::logic_error("unreachable");
            }
        }
    } else {
        throw std::logic_error("not implemented");
    }
}
llvm::Value* LLVMDefinitionCodegen::emit(const PrefixOperation& node) {
    stackGuard();

    auto* right{emit(*node.getRight())};
    switch (node.getOperationType()) {
        case LexemType::Minus:
            return getBuilder().CreateSub(llvm::ConstantInt::get(right->getType(), 0), right);
        case LexemType::Not:
            return getBuilder().CreateNot(right);
        default:
            throw std::logic_error("unreachable");
    }
}
llvm::Value* LLVMDefinitionCodegen::emit(const CallFunction& node) {
    stackGuard();

    auto* function{getFunctions()[node.getSymbolRef()]};

    std::vector<llvm::Value*> arguments;
    arguments.reserve(node.getArguments().size());
    for (const auto& expression : node.getArguments()) {
        arguments.emplace_back(emit(*expression));
    }
    return getBuilder().CreateCall(function, arguments);
}
llvm::Value* LLVMDefinitionCodegen::emit(const Identifier& node) {
    stackGuard();

    auto* var{getObjects()[node.getSymbolRef()]};
    return getBuilder().CreateLoad(var->getAllocatedType(), var);
}
llvm::Value* LLVMDefinitionCodegen::emit(const Number& node) {
    stackGuard();
    return llvm::ConstantInt::get(getContext(),
                                  llvm::APInt(32, std::stoi(node.getNumber().getValue()), true));
}

void LLVMDefinitionCodegen::emit(const Statement& node) {
    stackGuard();

    switch (node.getKind()) {
        case Node::Kind::StatementBlock:
            emit(static_cast<const StatementsBlock&>(node));
            break;
        case Node::Kind::Assign:
            emit(static_cast<const Assign&>(node));
            break;
        case Node::Kind::Break:
            emit(static_cast<const Break&>(node));
            break;
        case Node::Kind::ExpressionStatement:
            emit(static_cast<const ExpressionStatement&>(node));
            break;
        case Node::Kind::If:
            emit(static_cast<const If&>(node));
            break;
        case Node::Kind::Return:
            emit(static_cast<const Return&>(node));
            break;
        case Node::Kind::LocalVarDeclaration:
            emit(static_cast<const LocalVariableDeclaration&>(node));
            break;
        case Node::Kind::While:
            emit(static_cast<const While&>(node));
            break;
        default:
            throw std::logic_error("unreachable");
    }
}
void LLVMDefinitionCodegen::emit(const Assign& node) {
    stackGuard();

    auto* value{emit(*node.getValue())};
    getBuilder().CreateStore(value, getObjects()[node.getSymbolRef()]);
}
void LLVMDefinitionCodegen::emit([[maybe_unused]] const Break& node) {
    stackGuard();

    if (mCycles.empty()) {
        throw std::logic_error("unexpected");
    }

    getBuilder().CreateBr(mCycles.top().getMergeBlock());
    auto* continueBlock{llvm::BasicBlock::Create(getContext())};
    setBlock(continueBlock);
}
void LLVMDefinitionCodegen::emit(const ExpressionStatement& node) {
    stackGuard();

    emit(*node.getExpression());
}
void LLVMDefinitionCodegen::emit(const If& node) {
    stackGuard();

    auto* condition{emit(*node.getCondition())};
    auto* thenBlock{llvm::BasicBlock::Create(getContext())};
    auto* mergeBlock{llvm::BasicBlock::Create(getContext())};
    llvm::BasicBlock* elseBlock{nullptr};

    if (node.getElseThen()) {
        elseBlock = llvm::BasicBlock::Create(getContext());
        getBuilder().CreateCondBr(condition, thenBlock, elseBlock);
    } else {
        getBuilder().CreateCondBr(condition, thenBlock, mergeBlock);
    }

    setBlock(thenBlock);
    emit(*node.getThen());
    if (!getBuilder().GetInsertBlock()->getTerminator()) {
        getBuilder().CreateBr(mergeBlock);
    }

    if (elseBlock) {
        setBlock(elseBlock);
        emit(*node.getElseThen());
        if (!getBuilder().GetInsertBlock()->getTerminator()) {
            getBuilder().CreateBr(mergeBlock);
        }
    }

    setBlock(mergeBlock);
}
void LLVMDefinitionCodegen::emit(const LocalVariableDeclaration& node) {
    stackGuard();

    getObjects()[node.getSymbolRef()]
        = getBuilder().CreateAlloca(getTypes()[node.getSymbolRef()->getTypeSymbol()->getType()]);
    auto* value{emit(*node.getValue())};
    getBuilder().CreateStore(value, getObjects()[node.getSymbolRef()]);
}
void LLVMDefinitionCodegen::emit(const Return& node) {
    stackGuard();

    getBuilder().CreateRet(emit(*node.getReturnExpression()));
    setBlock(llvm::BasicBlock::Create(getContext()));
}
void LLVMDefinitionCodegen::emit(const StatementsBlock& node) {
    stackGuard();

    for (const auto& statement : node.getStatements()) {
        emit(*statement);
    }
}
void LLVMDefinitionCodegen::emit(const While& node) {
    stackGuard();

    auto* conditionBlock{llvm::BasicBlock::Create(getContext())};
    auto* bodyBlock{llvm::BasicBlock::Create(getContext())};
    auto* mergeBlock{llvm::BasicBlock::Create(getContext())};

    getBuilder().CreateBr(conditionBlock);
    setBlock(conditionBlock);
    getBuilder().CreateCondBr(emit(*node.getCondition()), bodyBlock, mergeBlock);

    mCycles.emplace(mergeBlock);
    setBlock(bodyBlock);
    emit(*node.getBody());
    getBuilder().CreateBr(conditionBlock);
    mCycles.pop();

    setBlock(mergeBlock);
}

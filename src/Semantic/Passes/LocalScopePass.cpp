// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LocalScopePass.hpp"

#include "AST/ASTNodes.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"
#include "Semantic/Types/Type.hpp"

void LocalScopePass::analyze() {
    stackGuard();

    visit(mAst);
}
void LocalScopePass::visit(Node& node) {
    stackGuard();

    if (!node.isInvalid()) {
        node.accept(*this);
    }
}

void LocalScopePass::visit(ModuleDeclaration& node) {
    stackGuard();

    mContext.getSymbolTable().openScope();
    const auto result = mContext.getSymbolTable().insertSymbol(*node.symbolRef);
    if (result.kind != SymbolTable::InsertResult::Kind::Successful) {
        if (result.kind == SymbolTable::InsertResult::Kind::NameConflict) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("redefinition of '{}'", node.name),
                                                    {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        } else if (result.kind == SymbolTable::InsertResult::Kind::ProhibitedShadowing) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("shadowing of '{}' is prohibited", node.name),
                                  {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
    }
    visit(*node.body);
    mContext.getSymbolTable().closeScope();
}
void LocalScopePass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.declarations) {
        if (!declaration->isInvalid() && declaration->getSymbolPtr()) {
            mContext.getSymbolTable().insertSymbol(*declaration->getSymbolPtr());
        }
    }
    for (auto& declaration : node.declarations) {
        visit(*declaration);
    }
}
void LocalScopePass::visit(FunctionDeclaration& node) {
    stackGuard();

    mCurrentFunction = &node;

    mContext.getSymbolTable().openScope();
    for (const auto& parameter : node.parameters) {
        visit(*parameter);
    }
    visit(*node.definition);
    mContext.getSymbolTable().closeScope();

    mCurrentFunction = nullptr;
}
void LocalScopePass::visit(FunctionParameter& node) {
    stackGuard();

    mContext.getSymbolTable().insertSymbol(*node.symbolRef);
}

void LocalScopePass::visit(Assign& node) {
    stackGuard();

    visit(*node.value);

    const auto result = mContext.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("can't find '{}'", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Variable) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't variable", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    node.symbolRef = static_cast<LocalVariableSymbol*>(&result.record->symbol);

    if (*node.symbolRef->getTypeSymbol()->getType() != *node.value->exprType) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}

void LocalScopePass::visit(CallFunction& node) {
    stackGuard();

    const auto result = mContext.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared function '{}'", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Function) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't function", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    auto& functionSymbol = static_cast<FunctionSymbol&>(result.record->symbol);

    node.symbolRef = &functionSymbol;
    node.exprType = functionSymbol.getTypeSymbol()->getType();

    if (node.arguments.size() != functionSymbol.getArguments().size()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("different argument count", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    size_t index = 0;
    for (const auto& argument : node.arguments) {
        visit(*argument);
        if (argument->exprType
            != functionSymbol.getArguments()[index]->getTypeSymbol()->getType()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("incompatible types of arguments", node.name),
                                  {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
        ++index;
    }
}
void LocalScopePass::visit(Number& node) {
    stackGuard();

    node.exprType = mContext.getIntType(32, true)->getType();
}
void LocalScopePass::visit(Identifier& node) {
    stackGuard();

    const auto result = mContext.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared variable '{}'", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Variable) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't variable", node.name),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    node.symbolRef = static_cast<LocalVariableSymbol*>(&result.record->symbol);
    node.exprType = node.symbolRef->getTypeSymbol()->getType();
}
void LocalScopePass::visit(BinaryOperation& node) {
    stackGuard();

    visit(*node.left);
    visit(*node.right);

    const auto checkResult
        = node.left->exprType->binaryOperationType(node.operationType, *node.right->exprType);

    if (checkResult.isTypesCorrect) {
        node.exprType = checkResult.resultType;
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}

void LocalScopePass::visit(PrefixOperation& node) {
    stackGuard();

    visit(*node.right);

    const auto checkResult = node.right->exprType->prefixOperationType(node.operationType);

    if (checkResult.isTypesCorrect) {
        node.exprType = checkResult.resultType;
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}
void LocalScopePass::visit(LocalVariableDeclaration& node) {
    stackGuard();

    const auto typeFindResult = mContext.getSymbolTable().findSymbol(node.type);

    if (typeFindResult.kind != SymbolTable::FindResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared type {}", node.type),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (typeFindResult.record->symbol.getKind() != Symbol::Kind::Type) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("{} isn't type", node.type),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    const auto symbol = mContext.addSymbol<LocalVariableSymbol>(node.name);
    symbol->setTypeSymbol(static_cast<TypeSymbol*>(&typeFindResult.record->symbol));
    node.symbolRef = symbol;

    const auto insertResult = mContext.getSymbolTable().insertSymbol(*node.symbolRef);
    if (insertResult.kind != SymbolTable::InsertResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared type {}", node.type),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    if (node.value != nullptr) {
        visit(*node.value);

        if (*node.value->exprType != *node.symbolRef->getTypeSymbol()->getType()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("incompatible types"),
                                                    {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
    }
}
void LocalScopePass::visit(StatementsBlock& node) {
    stackGuard();

    for (const auto& stmt : node.statements) {
        if (stmt->kind == Node::Kind::StatementBlock) {
            mContext.getSymbolTable().openScope();
            visit(*stmt);
            mContext.getSymbolTable().closeScope();
        } else {
            visit(*stmt);
        }
    }
}
void LocalScopePass::visit(Break& node) {
    stackGuard();

    if (mLoops.empty()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("break outisde loop"),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    node.breakedStmt = mLoops.top();
}
void LocalScopePass::visit(If& node) {
    stackGuard();

    visit(*node.condition);

    mContext.getSymbolTable().openScope();
    visit(*node.then);
    mContext.getSymbolTable().closeScope();

    if (node.elseThen != nullptr) {
        mContext.getSymbolTable().openScope();
        visit(*node.elseThen);
        mContext.getSymbolTable().closeScope();
    }
}
void LocalScopePass::visit(While& node) {
    stackGuard();

    mLoops.push(&node);
    mContext.getSymbolTable().openScope();

    visit(*node.condition);
    for (const auto& stmt : node.body->statements) {
        visit(*stmt);
    }

    mLoops.pop();
    mContext.getSymbolTable().closeScope();
}
void LocalScopePass::visit(Print& node) {
    stackGuard();

    visit(*node.expression);
}
void LocalScopePass::visit(Return& node) {
    stackGuard();

    visit(*node.returnExpression);
    if (node.returnExpression->exprType
        != mCurrentFunction->symbolRef->getTypeSymbol()->getType()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("bad return type"),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}

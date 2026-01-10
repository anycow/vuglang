// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LocalScopePass.hpp"

#include <cstddef>
#include <format>

#include "AST/Declarations.hpp"
#include "AST/Expressions.hpp"
#include "AST/Node.hpp"
#include "AST/Statements.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/Symbol.hpp"
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

    const auto result = mContext.getSymbolTable().insertSymbol(*node.getSymbolPtr());
    if (result.isError()) {
        if (result.error().kind == SymbolTable::InsertError::Kind::NameConflict) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("redefinition of '{}'", node.getName().getValue()),
                                  {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        } else if (result.error().kind == SymbolTable::InsertError::Kind::ProhibitedShadowing) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(
                DiagnosticMessage::Severity::Error,
                std::format("shadowing of '{}' is prohibited", node.getName().getValue()),
                {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
    }

    mContext.getSymbolTable().openScope();
    visit(*node.getBody());
    mContext.getSymbolTable().closeScope();
}
void LocalScopePass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.getDeclarations()) {
        if (declaration->isInvalid() || !declaration->getSymbolPtr()) {
            continue;
        }

        const auto result = mContext.getSymbolTable().insertSymbol(*declaration->getSymbolPtr());
        if (result.isError()) {
            if (result.error().kind == SymbolTable::InsertError::Kind::NameConflict) {
                auto diagnostic = Diagnostic();
                diagnostic.addMessage(DiagnosticMessage(
                    DiagnosticMessage::Severity::Error,
                    std::format("redefinition of '{}'", declaration->getSymbolPtr()->getName()),
                    {node.getSourceLocation()}));
                mDiagnosticManager.report(diagnostic);
                return;
            } else if (result.error().kind == SymbolTable::InsertError::Kind::ProhibitedShadowing) {
                auto diagnostic = Diagnostic();
                diagnostic.addMessage(
                    DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                      std::format("shadowing of '{}' is prohibited",
                                                  declaration->getSymbolPtr()->getName()),
                                      {node.getSourceLocation()}));
                mDiagnosticManager.report(diagnostic);
                return;
            }
        }
    }
    for (const auto& declaration : node.getDeclarations()) {
        visit(*declaration);
    }
}
void LocalScopePass::visit(FunctionDeclaration& node) {
    stackGuard();

    if (node.getDefinition()) {
        mCurrentFunction = &node;

        mContext.getSymbolTable().openScope();
        for (const auto& parameter : node.getParameters()) {
            visit(*parameter);
        }
        visit(*node.getDefinition());
        mContext.getSymbolTable().closeScope();

        mCurrentFunction = nullptr;
    }
}
void LocalScopePass::visit(FunctionParameter& node) {
    stackGuard();

    const auto result = mContext.getSymbolTable().insertSymbol(*node.getSymbolPtr());
    if (result.isError()) {
        if (result.error().kind == SymbolTable::InsertError::Kind::NameConflict) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("redefinition of '{}'", node.getName().getValue()),
                                  {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        } else if (result.error().kind == SymbolTable::InsertError::Kind::ProhibitedShadowing) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(
                DiagnosticMessage::Severity::Error,
                std::format("shadowing of '{}' is prohibited", node.getName().getValue()),
                {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
    }
}

void LocalScopePass::visit(Assign& node) {
    stackGuard();

    visit(*node.getValue());

    const auto result = mContext.getSymbolTable().findSymbol(node.getName().getValue());
    if (result.isError()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("can't find '{}'", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (result.value()->symbol.getKind() != Symbol::Kind::Variable) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("'{}' isn't variable", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    node.setSymbolRef(static_cast<LocalVariableSymbol*>(&result.value()->symbol));

    if (*node.getVariableSymbolPtr()->getTypeSymbol()->getType()
        != *node.getValue()->getExprType()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}

void LocalScopePass::visit(CallFunction& node) {
    stackGuard();

    const auto result = mContext.getSymbolTable().findSymbol(node.getName().getValue());

    if (result.isError()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("undeclared function '{}'", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (result.value()->symbol.getKind() != Symbol::Kind::Function) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("'{}' isn't function", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    auto& functionSymbol = static_cast<FunctionSymbol&>(result.value()->symbol);

    node.setSymbolRef(&functionSymbol);
    node.setExprType(functionSymbol.getTypeSymbol()->getType());

    if (node.getArguments().size() != functionSymbol.getArguments().size()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("different argument count", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    size_t index = 0;
    for (const auto& argument : node.getArguments()) {
        visit(*argument);
        if (argument->getExprType()
            != functionSymbol.getArguments()[index]->getTypeSymbol()->getType()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(
                DiagnosticMessage::Severity::Error,
                std::format("incompatible types of arguments", node.getName().getValue()),
                {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
        ++index;
    }
}
void LocalScopePass::visit(Number& node) {
    stackGuard();

    node.setExprType(mContext.getIntType(32, true)->getType());
}
void LocalScopePass::visit(Identifier& node) {
    stackGuard();

    const auto result = mContext.getSymbolTable().findSymbol(node.getName().getValue());

    if (result.isError()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("undeclared variable '{}'", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (result.value()->symbol.getKind() != Symbol::Kind::Variable) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("'{}' isn't variable", node.getName().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    node.setSymbolRef(static_cast<LocalVariableSymbol*>(&result.value()->symbol));
    node.setExprType(node.getVariableSymbolPtr()->getTypeSymbol()->getType());
}
void LocalScopePass::visit(BinaryOperation& node) {
    stackGuard();

    visit(*node.getLeft());
    visit(*node.getRight());

    const auto checkResult
        = node.getLeft()->getExprType()->binaryOperationType(node.getOperationType(),
                                                             *node.getRight()->getExprType());

    if (checkResult.isTypesCorrect) {
        node.setExprType(checkResult.resultType);
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}

void LocalScopePass::visit(PrefixOperation& node) {
    stackGuard();

    visit(*node.getRight());

    const auto checkResult
        = node.getRight()->getExprType()->prefixOperationType(node.getOperationType());

    if (checkResult.isTypesCorrect) {
        node.setExprType(checkResult.resultType);
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}
void LocalScopePass::visit(LocalVariableDeclaration& node) {
    stackGuard();

    const auto typeFindResult = mContext.getSymbolTable().findSymbol(node.getType().getValue());

    if (typeFindResult.isError()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("undeclared type {}", node.getType().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    if (typeFindResult.value()->symbol.getKind() != Symbol::Kind::Type) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("{} isn't type", node.getType().getValue()),
                              {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    auto* const symbol = mContext.addSymbol<LocalVariableSymbol>(node.getName().getValue());
    symbol->setTypeSymbol(static_cast<TypeSymbol*>(&typeFindResult.value()->symbol));
    node.setSymbolRef(symbol);

    const auto insertResult = mContext.getSymbolTable().insertSymbol(*node.getVariableSymbolPtr());
    if (insertResult.isError()) {
        if (insertResult.error().kind == SymbolTable::InsertError::Kind::NameConflict) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("redefinition of '{}'", node.getName().getValue()),
                                  {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        } else if (insertResult.error().kind
                   == SymbolTable::InsertError::Kind::ProhibitedShadowing) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(
                DiagnosticMessage::Severity::Error,
                std::format("shadowing of '{}' is prohibited", node.getName().getValue()),
                {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
    }

    if (node.getValue() != nullptr) {
        visit(*node.getValue());

        if (*node.getValue()->getExprType()
            != *node.getVariableSymbolPtr()->getTypeSymbol()->getType()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("incompatible types"),
                                                    {node.getSourceLocation()}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
    }
}
void LocalScopePass::visit(StatementsBlock& node) {
    stackGuard();

    for (const auto& stmt : node.getStatements()) {
        if (stmt->getKind() == Node::Kind::StatementBlock) {
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
                                                {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
    node.setBreakedStmt(mLoops.top());
}
void LocalScopePass::visit(If& node) {
    stackGuard();

    visit(*node.getCondition());

    mContext.getSymbolTable().openScope();
    visit(*node.getThen());
    mContext.getSymbolTable().closeScope();

    if (node.getElseThen() != nullptr) {
        mContext.getSymbolTable().openScope();
        visit(*node.getElseThen());
        mContext.getSymbolTable().closeScope();
    }
}
void LocalScopePass::visit(While& node) {
    stackGuard();

    mLoops.push(&node);
    mContext.getSymbolTable().openScope();

    visit(*node.getCondition());
    for (const auto& stmt : node.getBody()->getStatements()) {
        visit(*stmt);
    }

    mLoops.pop();
    mContext.getSymbolTable().closeScope();
}
void LocalScopePass::visit(ExpressionStatement& node) {
    stackGuard();

    visit(*node.getExpression());
}
void LocalScopePass::visit(Return& node) {
    stackGuard();

    visit(*node.getReturnExpression());
    if (node.getReturnExpression()->getExprType()
        != mCurrentFunction->getSymbolPtr()->getTypeSymbol()->getType()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("bad return type"),
                                                {node.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
        return;
    }
}

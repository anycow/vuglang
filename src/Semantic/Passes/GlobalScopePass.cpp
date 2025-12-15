// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "GlobalScopePass.hpp"

#include <format>
#include <memory>

#include "AST/ASTNodes.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/Symbol.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"

void GlobalScopePass::analyze() {
    stackGuard();

    visit(mAst);
}

void GlobalScopePass::visit(Node& node) {
    stackGuard();

    if (!node.isInvalid()) {
        node.accept(*this);
    }
}

void GlobalScopePass::visit(ModuleDeclaration& node) {
    stackGuard();

    mContext.getSymbolTable().openScope();

    const auto result = mContext.getSymbolTable().insertSymbol(*node.getSymbolPtr());
    if (result.isError()) {
        if (result.error().kind == SymbolTable::InsertError::Kind::NameConflict) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("redefinition of '{}'", node.name),
                                                    {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        } else if (result.error().kind == SymbolTable::InsertError::Kind::ProhibitedShadowing) {
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
void GlobalScopePass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.declarations) {
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
                    {node.sourceLocation}));
                mDiagnosticManager.report(diagnostic);
                return;
            } else if (result.error().kind == SymbolTable::InsertError::Kind::ProhibitedShadowing) {
                auto diagnostic = Diagnostic();
                diagnostic.addMessage(
                    DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                      std::format("shadowing of '{}' is prohibited",
                                                  declaration->getSymbolPtr()->getName()),
                                      {node.sourceLocation}));
                mDiagnosticManager.report(diagnostic);
                return;
            }
        }
    }
    for (auto& declaration : node.declarations) {
        visit(*declaration);
    }
}
void GlobalScopePass::visit(FunctionDeclaration& node) {
    stackGuard();

    node.symbolRef->startDefinition();
    for (const auto& parameter : node.parameters) {
        auto* const parameterSymbol = mContext.addSymbol<LocalVariableSymbol>(parameter->name);

        parameterSymbol->startDefinition();
        const auto result = mContext.getSymbolTable().findSymbol(parameter->type);
        if (result.isError()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("Can't find '{}' type", parameter->type),
                                  {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        }

        if (result.value()->symbol.getKind() == Symbol::Kind::Type) {
            parameterSymbol->setTypeSymbol(static_cast<TypeSymbol*>(&result.value()->symbol));
        } else {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("'{}' isn't type", parameter->type),
                                                    {node.sourceLocation}));
            mDiagnosticManager.report(diagnostic);
            return;
        }
        parameterSymbol->finishDefinition();

        parameter->symbolRef = parameterSymbol;
        node.symbolRef->addArgument(*parameterSymbol);
    }

    const auto returnTypeRecord = mContext.getSymbolTable().findSymbol(node.returnType);
    if (returnTypeRecord.isError()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("Can't find '{}' type", node.returnType),
                              {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    if (returnTypeRecord.value()->symbol.getKind() == Symbol::Kind::Type) {
        node.symbolRef->setTypeSymbol(static_cast<TypeSymbol*>(&returnTypeRecord.value()->symbol));
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't type", node.returnType),
                                                {node.sourceLocation}));
        mDiagnosticManager.report(diagnostic);
        return;
    }

    if (node.definition) {
        node.symbolRef->setDefinition(*node.definition);
    }
    node.symbolRef->finishDefinition();
}

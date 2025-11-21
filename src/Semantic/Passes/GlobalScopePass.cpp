// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "GlobalScopePass.hpp"

#include "AST/ASTNodes.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"


void GlobalScopePass::analyze() {
    stackGuard();

    visit(_ast);
}

void GlobalScopePass::visit(Node& node) {
    stackGuard();

    if (!node.isInvalid()) {
        node.accept(*this);
    }
}

void GlobalScopePass::visit(ModuleDeclaration& node) {
    stackGuard();

    _context.getSymbolTable().openScope();
    _context.getSymbolTable().insertSymbol(*node.getSymbolPtr());
    visit(*node.body);
    _context.getSymbolTable().closeScope();
}
void GlobalScopePass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.declarations) {
        if (!declaration->isInvalid() && declaration->getSymbolPtr()) {
            _context.getSymbolTable().insertSymbol(*declaration->getSymbolPtr());
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
        const auto parameterSymbol = _context.addSymbol<LocalVariableSymbol>(parameter->name);

        parameterSymbol->startDefinition();
        const auto parameterTypeRecord = _context.getSymbolTable().findSymbol(parameter->type);
        if (parameterTypeRecord.kind == SymbolTable::FindResult::Kind::Successful) {
            if (parameterTypeRecord.record->symbol.getKind() == Symbol::Kind::Type) {
                parameterSymbol->setTypeSymbol(
                    static_cast<TypeSymbol*>(&parameterTypeRecord.record->symbol));
            } else {
                auto diagnostic = Diagnostic();
                diagnostic.addMessage(
                    DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                      std::format("'{}' isn't type", parameter->type),
                                      {node.sourceLocation}));
                _diagnosticManager.report(diagnostic);
                return;
            }
        } else {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(
                DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                  std::format("Can't find '{}' type", parameter->type),
                                  {node.sourceLocation}));
            _diagnosticManager.report(diagnostic);
            return;
        }
        parameterSymbol->finishDefinition();

        parameter->symbolRef = parameterSymbol;
        node.symbolRef->addArgument(*parameterSymbol);
    }

    const auto returnTypeRecord = _context.getSymbolTable().findSymbol(node.returnType);
    if (returnTypeRecord.kind == SymbolTable::FindResult::Kind::Successful) {
        if (returnTypeRecord.record->symbol.getKind() == Symbol::Kind::Type) {
            node.symbolRef->setTypeSymbol(
                static_cast<TypeSymbol*>(&returnTypeRecord.record->symbol));
        } else {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("'{}' isn't type", node.returnType),
                                                    {node.sourceLocation}));
            _diagnosticManager.report(diagnostic);
            return;
        }
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(
            DiagnosticMessage(DiagnosticMessage::Severity::Error,
                              std::format("Can't find '{}' type", node.returnType),
                              {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }

    node.symbolRef->setDefinition(*node.definition);
    node.symbolRef->finishDefinition();
}

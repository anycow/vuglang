// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GlobalScopePass.hpp"

#include "GlobalScopePass.hpp"

#include "AST/ASTNodes.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"


void GlobalScopePass::analyze() {
    stackGuard();

    visit(_ast);
}

void GlobalScopePass::visit(Node& node) {
    stackGuard();

    node.accept(*this);
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

    for (auto& declaration: node.declarations) {
        _context.getSymbolTable().insertSymbol(*declaration->getSymbolPtr());
    }
    for (auto& declaration: node.declarations) {
        visit(*declaration);
    }
}
void GlobalScopePass::visit(FunctionDeclaration& node) {
    stackGuard();

    node.symbolRef->startDefinition();
    for (const auto& parameter: node.parameters) {
        auto parameterSymbol = _context.addSymbol<LocalVariableSymbol>(parameter->name);

        parameterSymbol->startDefinition();
        auto parameterTypeRecord = _context.getSymbolTable().findSymbol(parameter->type);
        if (parameterTypeRecord.kind == SymbolTable::FindResult::Kind::Successful) {
            if (parameterTypeRecord.record->symbol.getKind() ==
                Symbol::Kind::Type) {
                parameterSymbol->setTypeSymbol(static_cast<TypeSymbol*>(&parameterTypeRecord.record->symbol));
            } else {
                throw std::logic_error("Symbol isn't type");
            }
        } else {
            throw std::logic_error("Can't find symbol");
        }
        parameterSymbol->finishDefinition();

        parameter->symbolRef = parameterSymbol;
        node.symbolRef->addArgument(*parameterSymbol);
    }

    auto returnTypeRecord = _context.getSymbolTable().findSymbol(node.returnType);
    if (returnTypeRecord.kind == SymbolTable::FindResult::Kind::Successful) {
        if (returnTypeRecord.record->symbol.getKind() == Symbol::Kind::Type) {
            node.symbolRef->setTypeSymbol(static_cast<TypeSymbol*>(&returnTypeRecord.record->symbol));
        } else {
            throw std::logic_error("Symbol isn't type");
        }
    } else {
        throw std::logic_error("Can't find symbol");
    }

    node.symbolRef->setDefinition(*node.definition);
    node.symbolRef->finishDefinition();
}

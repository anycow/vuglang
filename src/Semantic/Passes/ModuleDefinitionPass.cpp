// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "ModuleDefinitionPass.hpp"

#include "AST/ASTNodes.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"


void ModuleDefinitionPass::analyze() {
    stackGuard();

    visit(mAst);
}

void ModuleDefinitionPass::visit(Node& node) {
    stackGuard();

    if (!node.isInvalid()) {
        node.accept(*this);
    }
}

void ModuleDefinitionPass::visit(ModuleDeclaration& node) {
    stackGuard();

    const auto module = mContext.addSymbol<ModuleSymbol>(node.name);
    node.symbolRef = module;

    module->startDefinition();
    visit(*node.body);
    for (const auto& declaration : node.body->declarations) {
        if (!declaration->isInvalid() && declaration->getSymbolPtr()) {
            module->addMember(*declaration->getSymbolPtr());
        }
    }
    module->finishDefinition();
}
void ModuleDefinitionPass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (auto& declaration : node.declarations) {
        visit(*declaration);
    }
}
void ModuleDefinitionPass::visit(FunctionDeclaration& node) {
    stackGuard();

    const auto function = mContext.addSymbol<FunctionSymbol>(node.name);
    node.symbolRef = function;
}

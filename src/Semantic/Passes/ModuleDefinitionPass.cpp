// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "ModuleDefinitionPass.hpp"

#include "AST/Declarations.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/Symbol.hpp"
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

    auto* const module = mContext.addSymbol<ModuleSymbol>(node.getName().getValue());
    node.setSymbolRef(module);

    module->startDefinition();
    visit(*node.getBody());
    for (const auto& declaration : node.getBody()->getDeclarations()) {
        if (!declaration->isInvalid() && declaration->getSymbolPtr()) {
            module->addMember(*declaration->getSymbolPtr());
        }
    }
    module->finishDefinition();
}
void ModuleDefinitionPass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.getDeclarations()) {
        visit(*declaration);
    }
}
void ModuleDefinitionPass::visit(FunctionDeclaration& node) {
    stackGuard();

    auto* const function = mContext.addSymbol<FunctionSymbol>(node.getName().getValue());
    node.setSymbolRef(function);
}

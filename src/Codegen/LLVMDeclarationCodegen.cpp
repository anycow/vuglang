// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LLVMDeclarationCodegen.hpp"

#include "AST/ASTNodes.hpp"
#include "Codegen/LLVMCodegen.hpp"
#include "Misc/Stack.hpp"

void LLVMDeclarationCodegen::emit(const ModuleDeclaration& node) {
    stackGuard();

    emit(*node.body);
}

void LLVMDeclarationCodegen::emit(const Declaration& node) {
    stackGuard();

    switch (node.kind) {
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

void LLVMDeclarationCodegen::emit(const DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.declarations) {
        emit(*declaration);
    }
}

void LLVMDeclarationCodegen::emit(const FunctionDeclaration& node) {
    stackGuard();

    std::vector<llvm::Type*> parameters;
    for (auto& parameter : node.parameters) {
        parameters.emplace_back(
            getTypes()[parameter->symbolRef->getTypeSymbol()->getType()]);
    }

    auto* functionType
        = llvm::FunctionType::get(getTypes()[node.symbolRef->getTypeSymbol()->getType()],
                                  parameters,
                                  false);
    auto* function = llvm::Function::Create(functionType,
                                            llvm::Function::ExternalLinkage,
                                            node.name,
                                            &getModule());

    getFunctions()[node.symbolRef] = function;
}

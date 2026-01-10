// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "LLVMDeclarationCodegen.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <stdexcept>
#include <vector>

#include "AST/Declarations.hpp"
#include "Codegen/LLVMCodegen.hpp"
#include "Misc/Stack.hpp"

void LLVMDeclarationCodegen::emit(const ModuleDeclaration& node) {
    stackGuard();

    emit(*node.getBody());
}

void LLVMDeclarationCodegen::emit(const Declaration& node) {
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

void LLVMDeclarationCodegen::emit(const DeclarationsBlock& node) {
    stackGuard();

    for (const auto& declaration : node.getDeclarations()) {
        emit(*declaration);
    }
}

void LLVMDeclarationCodegen::emit(const FunctionDeclaration& node) {
    stackGuard();

    std::vector<llvm::Type*> parameters;
    parameters.reserve(node.getParameters().size());
    for (const auto& parameter : node.getParameters()) {
        parameters.emplace_back(getTypes()[parameter->getSymbolRef()->getTypeSymbol()->getType()]);
    }

    auto* functionType
        = llvm::FunctionType::get(getTypes()[node.getSymbolRef()->getTypeSymbol()->getType()],
                                  parameters,
                                  false);
    auto* function = llvm::Function::Create(functionType,
                                            llvm::Function::ExternalLinkage,
                                            node.getName().getValue(),
                                            &getModule());

    getFunctions()[node.getSymbolRef()] = function;
}

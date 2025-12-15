// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_FUNCTIONDECLARATION_HPP
#define VUG_FUNCTIONDECLARATION_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Declaration.hpp"
#include "AST/Nodes/Declarations/FunctionParameter.hpp"
#include "AST/Nodes/Statements/StatementsBlock.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

struct FunctionDeclaration : public Declaration {
    std::string name;
    std::string returnType;
    std::vector<std::unique_ptr<FunctionParameter>> parameters;
    std::unique_ptr<StatementsBlock> definition;

    FunctionSymbol* symbolRef{nullptr};

    FunctionDeclaration(std::string name,
                        std::string returnType,
                        std::vector<std::unique_ptr<FunctionParameter>> parameters,
                        std::unique_ptr<StatementsBlock> definition,
                        const SourceLocation& sourceLocation)
        : Declaration(Kind::FunctionDeclaration, sourceLocation),
          name(std::move(name)),
          returnType(std::move(returnType)),
          parameters(std::move(parameters)),
          definition(std::move(definition)) {
    }

    [[nodiscard]] FunctionSymbol* getSymbolPtr() const override {
        return symbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif  // VUG_FUNCTIONDECLARATION_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_FUNCTIONDECLARATION_HPP
#define VUG_FUNCTIONDECLARATION_HPP

#include "AST/Nodes/Declaration.hpp"

#include <utility>

#include "AST/Nodes/Declarations/FunctionParameter.hpp"
#include "AST/Nodes/Expressions/Identifier.hpp"
#include "AST/Nodes/Statements/StatementsBlock.hpp"

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
                        SourceLocation sourceLocation)
        : Declaration(Kind::FunctionDeclaration, sourceLocation),
          name(std::move(name)),
          returnType(std::move(returnType)),
          parameters(std::move(parameters)),
          definition(std::move(definition)) {
    }

    [[nodiscard]] Symbol* getSymbolPtr() const override {
        return symbolRef;
    }

    void evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateDeclaration(*this);
    }
    void accept(ASTWalker& walker) override {
        return walker.visit(*this);
    }
};

#endif  // VUG_FUNCTIONDECLARATION_HPP

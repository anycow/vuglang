// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_PARAMETER_HPP
#define VUG_PARAMETER_HPP

#include "AST/Nodes/Declaration.hpp"

#include "AST/Nodes/Expressions/Identifier.hpp"

struct FunctionParameter : public Declaration {
    std::string type;
    std::string name;
    std::unique_ptr<Expression> defaultValue;

    LocalVariableSymbol* symbolRef{nullptr};

    FunctionParameter(std::string type,
                      std::string name,
                      SourceLocation sourceLocation)
        : Declaration(Kind::FunctionParameter, sourceLocation),
          type(std::move(type)),
          name(std::move(name)) {}

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

#endif//VUG_PARAMETER_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_IDENTIFIER_HPP
#define VUG_IDENTIFIER_HPP

#include <string>

#include "AST/Nodes/Expression.hpp"

struct Identifier : public Expression {
    std::string name;

    LocalVariableSymbol* symbolRef{nullptr};

    explicit Identifier(std::string name)
        : Expression(Kind::Identifier),
          name(std::move(name)) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    std::unique_ptr<Object> evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateExpression(*this);
    }
};

#endif//VUG_IDENTIFIER_HPP

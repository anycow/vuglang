// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_NUMBER_HPP
#define VUG_NUMBER_HPP

#include <string>
#include <utility>

#include "AST/Nodes/Expression.hpp"

struct Number : public Expression {
    std::string number;

    Number(std::string num, const SourceLocation& sourceLocation)
        : Expression(Kind::Number, sourceLocation),
          number(std::move(num)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    std::unique_ptr<Object> evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateExpression(*this);
    }
};


#endif  // VUG_NUMBER_HPP

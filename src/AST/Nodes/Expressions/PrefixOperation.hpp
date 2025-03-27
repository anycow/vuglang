// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_PREFIXOPERATION_HPP
#define VUG_PREFIXOPERATION_HPP

#include "AST/Nodes/Expression.hpp"

struct PrefixOperation : public Expression {
    LexemType op;
    std::unique_ptr<Expression> right;

    PrefixOperation(LexemType op, std::unique_ptr<Expression> right)
        : Expression(Kind::PrefixOperation),
          op(op),
          right(std::move(right)) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    std::unique_ptr<Object> evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateExpression(*this);
    }
};


#endif//VUG_PREFIXOPERATION_HPP

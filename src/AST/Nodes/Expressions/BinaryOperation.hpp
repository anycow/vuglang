// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_BINARYOPERATION_HPP
#define VUG_BINARYOPERATION_HPP

#include <utility>

#include "AST/Nodes/Expression.hpp"

struct BinaryOperation : public Expression {
    LexemType operationToken;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryOperation(const LexemType operationToken,
                    std::unique_ptr<Expression> left,
                    std::unique_ptr<Expression> right,
                    const SourceLocation& sourceLocation)
        : Expression(Kind::BinaryOperation, sourceLocation),
          operationToken(operationToken),
          left(std::move(left)),
          right(std::move(right)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    std::unique_ptr<Object> evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateExpression(*this);
    }
};


#endif  // VUG_BINARYOPERATION_HPP

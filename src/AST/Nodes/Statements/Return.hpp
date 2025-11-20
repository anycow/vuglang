// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_RETURN_HPP
#define VUG_RETURN_HPP

#include "AST/Nodes/Expression.hpp"
#include "AST/Nodes/Statement.hpp"

struct Return : public Statement {
    std::unique_ptr<Expression> returnExpression;

    Return(std::unique_ptr<Expression> returnedExpression,
           SourceLocation sourceLocation)
        : Statement(Kind::Return, sourceLocation),
          returnExpression(std::move(returnedExpression)) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    StmtResult evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateStatement(*this);
    }
};

#endif//VUG_RETURN_HPP

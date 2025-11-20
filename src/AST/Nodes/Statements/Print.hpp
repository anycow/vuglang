// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_PRINT_HPP
#define VUG_PRINT_HPP

#include "AST/Nodes/Expression.hpp"
#include "AST/Nodes/Statement.hpp"

struct Print : public Statement {
    std::unique_ptr<Expression> expression;

    Print(std::unique_ptr<Expression> expression, SourceLocation sourceLocation)
        : Statement(Kind::Print, sourceLocation),
          expression(std::move(expression)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    StmtResult evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateStatement(*this);
    }
};

#endif  // VUG_PRINT_HPP

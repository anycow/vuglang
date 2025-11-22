// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_IF_HPP
#define VUG_IF_HPP

#include "AST/Nodes/Statement.hpp"

#include "AST/Nodes/Expression.hpp"
#include "StatementsBlock.hpp"

struct If : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<StatementsBlock> then;
    std::unique_ptr<Statement> elseThen;

    If(std::unique_ptr<Expression> condition,
       std::unique_ptr<StatementsBlock> then,
       std::unique_ptr<Statement> elseThen,
       const SourceLocation& sourceLocation)
        : Statement(Kind::If, sourceLocation),
          condition(std::move(condition)),
          then(std::move(then)),
          elseThen(std::move(elseThen)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    StmtResult evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateStatement(*this);
    }
};

#endif  // VUG_IF_HPP

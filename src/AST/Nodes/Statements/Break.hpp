// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_BREAK_HPP
#define VUG_BREAK_HPP

#include "AST/Nodes/Statement.hpp"

struct Break : public Statement {
    Statement* breakedStmt{nullptr};

    explicit Break(SourceLocation sourceLocation)
        : Statement(Kind::Break, sourceLocation) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    StmtResult evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateStatement(*this);
    }
};

#endif//VUG_BREAK_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_BREAK_HPP
#define VUG_BREAK_HPP

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Statement.hpp"
#include "Lexing/Token.hpp"

struct Break : public Statement {
    Statement* breakedStmt{nullptr};

    explicit Break(const SourceLocation& sourceLocation)
        : Statement(Kind::Break, sourceLocation) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif  // VUG_BREAK_HPP

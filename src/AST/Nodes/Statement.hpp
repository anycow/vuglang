// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_STATEMENT_HPP
#define VUG_STATEMENT_HPP

#include "Node.hpp"

struct Statement : public Node {
    Statement(const Kind nodeType, const SourceLocation& sourceLocation)
        : Node(nodeType, sourceLocation) {
    }

    bool isStatement() const override {
        return true;
    }
};

struct BadStatement : public Statement {
    BadStatement()
        : Statement(Kind::BadStatement, SourceLocation()) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    [[nodiscard]] bool isInvalid() const override {
        return true;
    }
};

#endif  // VUG_STATEMENT_HPP

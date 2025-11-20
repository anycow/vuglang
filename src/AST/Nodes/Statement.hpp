// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_STATEMENT_HPP
#define VUG_STATEMENT_HPP

#include "Node.hpp"

#include "Evaluator/Evaluator.hpp"

struct Statement : public Node {
    explicit Statement(Kind nodeType, SourceLocation sourceLocation)
        : Node(nodeType, sourceLocation) {}

    bool isStatement() override { return true; }

    virtual StmtResult evaluate(Evaluator& evaluator) { throw std::logic_error("Not implemented"); }
};

struct BadStatement : public Statement {
    explicit BadStatement()
        : Statement(Kind::BadStatement, SourceLocation()) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    bool isInvalid() override {
        return true;
    }

    StmtResult evaluate(Evaluator& evaluator) override { throw std::logic_error("Not implemented"); }
};

#endif//VUG_STATEMENT_HPP

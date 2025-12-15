// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_STATEMENTSBLOCK_HPP
#define VUG_STATEMENTSBLOCK_HPP

#include <memory>
#include <utility>
#include <vector>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Statement.hpp"
#include "Lexing/Token.hpp"

struct StatementsBlock : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;

    StatementsBlock(std::vector<std::unique_ptr<Statement>> statements,
                    const SourceLocation& sourceLocation)
        : Statement(Kind::StatementBlock, sourceLocation),
          statements(std::move(statements)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif  // VUG_STATEMENTSBLOCK_HPP

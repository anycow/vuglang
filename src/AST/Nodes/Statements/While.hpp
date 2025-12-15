// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_WHILE_HPP
#define VUG_WHILE_HPP

#include <memory>
#include <utility>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Expression.hpp"
#include "AST/Nodes/Statement.hpp"
#include "AST/Nodes/Statements/StatementsBlock.hpp"
#include "Lexing/Token.hpp"

struct While : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<StatementsBlock> body;

    While(std::unique_ptr<Expression> condition,
          std::unique_ptr<StatementsBlock> body,
          const SourceLocation& sourceLocation)
        : Statement(Kind::While, sourceLocation),
          condition(std::move(condition)),
          body(std::move(body)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};


#endif  // VUG_WHILE_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_EXPRESSIONSTATEMENT_HPP
#define VUG_EXPRESSIONSTATEMENT_HPP

#include <memory>
#include <utility>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Expression.hpp"
#include "AST/Nodes/Statement.hpp"
#include "Lexing/Token.hpp"

struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expression;

    ExpressionStatement(std::unique_ptr<Expression> expression,
                        const SourceLocation& sourceLocation)
        : Statement(Kind::ExpressionStatement, sourceLocation),
          expression(std::move(expression)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif

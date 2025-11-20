// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_ASSIGN_HPP
#define VUG_ASSIGN_HPP

#include "AST/Nodes/Statement.hpp"

#include "AST/Nodes/Expressions/Identifier.hpp"

class LocalVariableSymbol;

struct Assign : public Statement {
    std::string name;
    std::unique_ptr<Expression> value;

    LocalVariableSymbol* symbolRef{nullptr};

    Assign(std::string name, std::unique_ptr<Expression> value, SourceLocation sourceLocation)
        : Statement(Kind::Assign, sourceLocation),
          name(std::move(name)),
          value(std::move(value)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    StmtResult evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateStatement(*this);
    }
};

#endif  // VUG_ASSIGN_HPP

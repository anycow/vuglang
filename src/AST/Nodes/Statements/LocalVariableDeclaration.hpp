// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_LOCALVARIABLEDECLARATION_HPP
#define VUG_LOCALVARIABLEDECLARATION_HPP

#include <utility>

#include "AST/Nodes/Expression.hpp"
#include "AST/Nodes/Expressions/Identifier.hpp"
#include "AST/Nodes/Statement.hpp"

#include "Semantic/Type.hpp"

struct LocalVariableDeclaration : public Statement {
    std::string type;
    std::string name;
    std::unique_ptr<Expression> value;

    LocalVariableSymbol* symbolRef{nullptr};

    LocalVariableDeclaration(std::string type,
                        std::string name,
                        std::unique_ptr<Expression> value = nullptr)
        : Statement(Kind::VarDeclaration),
          type(std::move(type)),
          name(std::move(name)),
          value(std::move(value)) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    StmtResult evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateStatement(*this);
    }
};
#endif//VUG_LOCALVARIABLEDECLARATION_HPP

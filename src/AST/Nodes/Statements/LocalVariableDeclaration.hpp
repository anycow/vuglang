// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_LOCALVARIABLEDECLARATION_HPP
#define VUG_LOCALVARIABLEDECLARATION_HPP

#include <memory>
#include <string>
#include <utility>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Expression.hpp"
#include "AST/Nodes/Statement.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

struct LocalVariableDeclaration : public Statement {
    std::string type;
    std::string name;
    std::unique_ptr<Expression> value;

    LocalVariableSymbol* symbolRef{nullptr};

    LocalVariableDeclaration(std::string type,
                             std::string name,
                             std::unique_ptr<Expression> value,
                             const SourceLocation& sourceLocation)
        : Statement(Kind::LocalVarDeclaration, sourceLocation),
          type(std::move(type)),
          name(std::move(name)),
          value(std::move(value)) {
    }

    [[nodiscard]] LocalVariableSymbol* getVariableSymbolPtr() const {
        return symbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};
#endif  // VUG_LOCALVARIABLEDECLARATION_HPP

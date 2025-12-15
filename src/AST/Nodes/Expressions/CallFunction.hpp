// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_CALLFUNCTION_HPP
#define VUG_CALLFUNCTION_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Expression.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

struct CallFunction : public Expression {
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionSymbol* symbolRef{nullptr};

    CallFunction(std::string name,
                 std::vector<std::unique_ptr<Expression>> expressions,
                 const SourceLocation& sourceLocation)
        : Expression(Kind::CallFunction, sourceLocation),
          name(std::move(name)),
          arguments(std::move(expressions)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif  // VUG_CALLFUNCTION_HPP

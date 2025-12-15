// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_PARAMETER_HPP
#define VUG_PARAMETER_HPP

#include <memory>
#include <string>
#include <utility>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Declaration.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

struct Expression;

struct FunctionParameter : public Declaration {
    std::string type;
    std::string name;
    std::unique_ptr<Expression> defaultValue;

    LocalVariableSymbol* symbolRef{nullptr};

    FunctionParameter(std::string type, std::string name, const SourceLocation& sourceLocation)
        : Declaration(Kind::FunctionParameter, sourceLocation),
          type(std::move(type)),
          name(std::move(name)) {
    }

    [[nodiscard]] LocalVariableSymbol* getSymbolPtr() const override {
        return symbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif  // VUG_PARAMETER_HPP

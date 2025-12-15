// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_PREFIXOPERATION_HPP
#define VUG_PREFIXOPERATION_HPP

#include <memory>
#include <utility>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Expression.hpp"
#include "Lexing/Token.hpp"

struct PrefixOperation : public Expression {
    LexemType operationType;
    std::unique_ptr<Expression> right;

    PrefixOperation(const LexemType operationType,
                    std::unique_ptr<Expression> right,
                    const SourceLocation& sourceLocation)
        : Expression(Kind::PrefixOperation, sourceLocation),
          operationType(operationType),
          right(std::move(right)) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};


#endif  // VUG_PREFIXOPERATION_HPP

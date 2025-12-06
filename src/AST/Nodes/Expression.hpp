// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_EXPRESSION_HPP
#define VUG_EXPRESSION_HPP

#include "Node.hpp"

class Type;
class Object;

struct Expression : public Node {
    const Type* exprType = nullptr;

    Expression(const Kind nodeType, const SourceLocation& sourceLocation)
        : Node(nodeType, sourceLocation) {
    }

    bool isExpression() const override {
        return true;
    }
};

struct BadExpression : public Expression {
    BadExpression()
        : Expression(Kind::BadExpression, SourceLocation()) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    [[nodiscard]] bool isInvalid() const override {
        return true;
    }
};

#endif  // VUG_EXPRESSION_HPP

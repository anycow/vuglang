// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_EXPRESSION_HPP
#define VUG_EXPRESSION_HPP

#include "Node.hpp"

#include "Evaluator/Evaluator.hpp"
#include "Evaluator/Objects/Object.hpp"

class Type;
class Object;

struct Expression : public Node {
    const Type* exprType = nullptr;

    explicit Expression(Kind nodeType,
                        SourceLocation sourceLocation)
        : Node(nodeType, sourceLocation) {}

    bool isExpression() override { return true; }

    virtual std::unique_ptr<Object> evaluate(Evaluator& evaluator) { throw std::logic_error("Not implemented"); }
};

struct BadExpression : public Expression {
    explicit BadExpression()
        : Expression(Kind::BadExpression, SourceLocation()) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    bool isInvalid() override {
        return true;
    }

    std::unique_ptr<Object> evaluate(Evaluator& evaluator) override { throw std::logic_error("Not implemented"); }
};

#endif//VUG_EXPRESSION_HPP
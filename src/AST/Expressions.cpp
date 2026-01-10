// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "AST/Expressions.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "AST/Node.hpp"
#include "Lexing/Token.hpp"

// Constructors and destructors moved to .cpp to avoid cyclic dependencies with unique_ptr
Expression::Expression(const Kind nodeType, const SourceLocation& sourceLocation)
    : Node(nodeType, sourceLocation) {
}

BadExpression::BadExpression()
    : Expression(Kind::BadExpression, SourceLocation()) {
}

BadExpression::~BadExpression() = default;

BinaryOperation::BinaryOperation(Token operation,
                                 std::unique_ptr<Expression> left,
                                 std::unique_ptr<Expression> right,
                                 const SourceLocation& sourceLocation)
    : Expression(Kind::BinaryOperation, sourceLocation),
      mOperation(std::move(operation)),
      mLeft(std::move(left)),
      mRight(std::move(right)) {
}

BinaryOperation::~BinaryOperation() = default;

CallFunction::CallFunction(Token name,
                           Token leftBracket,
                           Token rightBracket,
                           std::vector<std::unique_ptr<Expression>> expressions,
                           const SourceLocation& sourceLocation)
    : Expression(Kind::CallFunction, sourceLocation),
      mName(std::move(name)),
      mLeftBracket(std::move(leftBracket)),
      mRightBracket(std::move(rightBracket)),
      mArguments(std::move(expressions)) {
}

CallFunction::~CallFunction() = default;

Identifier::Identifier(Token name, const SourceLocation& sourceLocation)
    : Expression(Kind::Identifier, sourceLocation),
      mName(std::move(name)) {
}

Identifier::~Identifier() = default;

Number::Number(Token number, const SourceLocation& sourceLocation)
    : Expression(Kind::Number, sourceLocation),
      mNumber(std::move(number)) {
}

Number::~Number() = default;

PrefixOperation::PrefixOperation(Token operation,
                                 std::unique_ptr<Expression> right,
                                 const SourceLocation& sourceLocation)
    : Expression(Kind::PrefixOperation, sourceLocation),
      mOperation(std::move(operation)),
      mRight(std::move(right)) {
}

PrefixOperation::~PrefixOperation() = default;

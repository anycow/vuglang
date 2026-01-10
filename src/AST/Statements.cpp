// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "AST/Statements.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "AST/Expressions.hpp"
#include "AST/Node.hpp"
#include "Lexing/Token.hpp"

// Constructors and destructors moved to .cpp to avoid cyclic dependencies with unique_ptr
Statement::Statement(const Kind nodeType, const SourceLocation& sourceLocation)
    : Node(nodeType, sourceLocation) {
}

BadStatement::BadStatement()
    : Statement(Kind::BadStatement, SourceLocation()) {
}
BadStatement::~BadStatement() = default;

StatementsBlock::StatementsBlock(Token leftBracket,
                                 Token rightBracket,
                                 std::vector<std::unique_ptr<Statement>> statements,
                                 const SourceLocation& sourceLocation)
    : Statement(Kind::StatementBlock, sourceLocation),
      mLeftBracket(std::move(leftBracket)),
      mRightBracket(std::move(rightBracket)),
      mStatements(std::move(statements)) {
}
StatementsBlock::~StatementsBlock() = default;

Assign::Assign(Token name,
               Token assignToken,
               std::unique_ptr<Expression> value,
               const SourceLocation& sourceLocation)
    : Statement(Kind::Assign, sourceLocation),
      mName(std::move(name)),
      mAssignToken(std::move(assignToken)),
      mValue(std::move(value)) {
}
Assign::~Assign() = default;

Break::Break(Token breakKeyword, const SourceLocation& sourceLocation)
    : Statement(Kind::Break, sourceLocation),
      mBreakKeyword(std::move(breakKeyword)) {
}
Break::~Break() = default;

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression,
                                         const SourceLocation& sourceLocation)
    : Statement(Kind::ExpressionStatement, sourceLocation),
      mExpression(std::move(expression)) {
}
ExpressionStatement::~ExpressionStatement() = default;

If::If(Token ifKeyword,
       Token leftBracket,
       Token rightBracket,
       std::unique_ptr<Expression> condition,
       std::unique_ptr<StatementsBlock> then,
       std::unique_ptr<Statement> elseThen,
       const SourceLocation& sourceLocation)
    : Statement(Kind::If, sourceLocation),
      mIfKeyword(std::move(ifKeyword)),
      mLeftBracket(std::move(leftBracket)),
      mRightBracket(std::move(rightBracket)),
      mCondition(std::move(condition)),
      mThen(std::move(then)),
      mElseThen(std::move(elseThen)) {
}
If::~If() = default;

LocalVariableDeclaration::LocalVariableDeclaration(Token varKeyword,
                                                   Token type,
                                                   Token name,
                                                   Token assignToken,
                                                   std::unique_ptr<Expression> value,
                                                   const SourceLocation& sourceLocation)
    : Statement(Kind::LocalVarDeclaration, sourceLocation),
      mVarKeyword(std::move(varKeyword)),
      mType(std::move(type)),
      mName(std::move(name)),
      mAssignToken(std::move(assignToken)),
      mValue(std::move(value)) {
}
LocalVariableDeclaration::~LocalVariableDeclaration() = default;

Return::Return(Token returnKeyword,
               std::unique_ptr<Expression> returnedExpression,
               const SourceLocation& sourceLocation)
    : Statement(Kind::Return, sourceLocation),
      mReturnKeyword(std::move(returnKeyword)),
      mReturnExpression(std::move(returnedExpression)) {
}
Return::~Return() = default;

While::While(Token whileKeyword,
             Token leftBracket,
             Token rightBracket,
             std::unique_ptr<Expression> condition,
             std::unique_ptr<StatementsBlock> body,
             const SourceLocation& sourceLocation)
    : Statement(Kind::While, sourceLocation),
      mWhileKeyword(std::move(whileKeyword)),
      mLeftBracket(std::move(leftBracket)),
      mRightBracket(std::move(rightBracket)),
      mCondition(std::move(condition)),
      mBody(std::move(body)) {
}
While::~While() = default;

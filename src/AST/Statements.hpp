// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_STATEMENTS_HPP
#define VUG_STATEMENTS_HPP

#include <memory>
#include <vector>

#include "AST/ASTWalker.hpp"
#include "AST/Node.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

class Expression;

class Statement : public Node {
   public:
    Statement(Kind nodeType, const SourceLocation& sourceLocation);

    ~Statement() override = default;
    Statement(const Statement& other) = default;
    Statement(Statement&& other) noexcept = default;
    Statement& operator=(const Statement& other) = delete;
    Statement& operator=(Statement&& other) noexcept = delete;

    [[nodiscard]] bool isStatement() const override {
        return true;
    }
};

class BadStatement : public Statement {
   public:
    BadStatement();

    ~BadStatement() override;
    BadStatement(const BadStatement& other) = default;
    BadStatement(BadStatement&& other) noexcept = default;
    BadStatement& operator=(const BadStatement& other) = delete;
    BadStatement& operator=(BadStatement&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    [[nodiscard]] bool isInvalid() const override {
        return true;
    }
};

class StatementsBlock : public Statement {
   public:
    StatementsBlock(Token leftBracket,
                    Token rightBracket,
                    std::vector<std::unique_ptr<Statement>> statements,
                    const SourceLocation& sourceLocation);

    ~StatementsBlock() override;
    StatementsBlock(const StatementsBlock& other) = default;
    StatementsBlock(StatementsBlock&& other) noexcept = default;
    StatementsBlock& operator=(const StatementsBlock& other) = delete;
    StatementsBlock& operator=(StatementsBlock&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getLeftBracket() const {
        return mLeftBracket;
    }

    [[nodiscard]] const Token& getRightBracket() const {
        return mRightBracket;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Statement>>& getStatements() const {
        return mStatements;
    }

   private:
    Token mLeftBracket;
    Token mRightBracket;
    std::vector<std::unique_ptr<Statement>> mStatements;
};

class Assign : public Statement {
   public:
    Assign(Token name,
           Token assignToken,
           std::unique_ptr<Expression> value,
           const SourceLocation& sourceLocation);

    ~Assign() override;
    Assign(const Assign& other) = delete;
    Assign(Assign&& other) noexcept = default;
    Assign& operator=(const Assign& other) = delete;
    Assign& operator=(Assign&& other) noexcept = delete;

    [[nodiscard]] LocalVariableSymbol* getVariableSymbolPtr() const {
        return mSymbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getName() const {
        return mName;
    }

    [[nodiscard]] const Token& getAssignToken() const {
        return mAssignToken;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getValue() const {
        return mValue;
    }

    [[nodiscard]] LocalVariableSymbol* getSymbolRef() const {
        return mSymbolRef;
    }

    void setSymbolRef(LocalVariableSymbol* symbolRef) {
        mSymbolRef = symbolRef;
    }

   private:
    Token mName;
    Token mAssignToken;
    std::unique_ptr<Expression> mValue;
    LocalVariableSymbol* mSymbolRef{nullptr};
};

class Break : public Statement {
   public:
    explicit Break(Token breakKeyword, const SourceLocation& sourceLocation);

    ~Break() override;
    Break(const Break& other) = default;
    Break(Break&& other) noexcept = default;
    Break& operator=(const Break& other) = delete;
    Break& operator=(Break&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getBreakKeyword() const {
        return mBreakKeyword;
    }

    [[nodiscard]] Statement* getBreakedStmt() const {
        return mBreakedStmt;
    }

    void setBreakedStmt(Statement* breakedStmt) {
        mBreakedStmt = breakedStmt;
    }

   private:
    Token mBreakKeyword;
    Statement* mBreakedStmt{nullptr};
};

class ExpressionStatement : public Statement {
   public:
    ExpressionStatement(std::unique_ptr<Expression> expression,
                        const SourceLocation& sourceLocation);

    ~ExpressionStatement() override;
    ExpressionStatement(const ExpressionStatement& other) = delete;
    ExpressionStatement(ExpressionStatement&& other) noexcept = default;
    ExpressionStatement& operator=(const ExpressionStatement& other) = delete;
    ExpressionStatement& operator=(ExpressionStatement&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getExpression() const {
        return mExpression;
    }

   private:
    std::unique_ptr<Expression> mExpression;
};

class If : public Statement {
   public:
    If(Token ifKeyword,
       Token leftBracket,
       Token rightBracket,
       std::unique_ptr<Expression> condition,
       std::unique_ptr<StatementsBlock> then,
       std::unique_ptr<Statement> elseThen,
       const SourceLocation& sourceLocation);

    ~If() override;
    If(const If& other) = delete;
    If(If&& other) noexcept = default;
    If& operator=(const If& other) = delete;
    If& operator=(If&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getIfKeyword() const {
        return mIfKeyword;
    }

    [[nodiscard]] const Token& getLeftBracket() const {
        return mLeftBracket;
    }

    [[nodiscard]] const Token& getRightBracket() const {
        return mRightBracket;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getCondition() const {
        return mCondition;
    }

    [[nodiscard]] const std::unique_ptr<StatementsBlock>& getThen() const {
        return mThen;
    }

    [[nodiscard]] const std::unique_ptr<Statement>& getElseThen() const {
        return mElseThen;
    }

   private:
    Token mIfKeyword;
    Token mLeftBracket;
    Token mRightBracket;
    std::unique_ptr<Expression> mCondition;
    std::unique_ptr<StatementsBlock> mThen;
    std::unique_ptr<Statement> mElseThen;
};

class LocalVariableDeclaration : public Statement {
   public:
    LocalVariableDeclaration(Token varKeyword,
                             Token type,
                             Token name,
                             Token assignToken,
                             std::unique_ptr<Expression> value,
                             const SourceLocation& sourceLocation);

    ~LocalVariableDeclaration() override;
    LocalVariableDeclaration(const LocalVariableDeclaration& other) = delete;
    LocalVariableDeclaration(LocalVariableDeclaration&& other) noexcept = default;
    LocalVariableDeclaration& operator=(const LocalVariableDeclaration& other) = delete;
    LocalVariableDeclaration& operator=(LocalVariableDeclaration&& other) noexcept = delete;

    [[nodiscard]] LocalVariableSymbol* getVariableSymbolPtr() const {
        return mSymbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getVarKeyword() const {
        return mVarKeyword;
    }

    [[nodiscard]] const Token& getType() const {
        return mType;
    }

    [[nodiscard]] const Token& getName() const {
        return mName;
    }

    [[nodiscard]] const Token& getAssignToken() const {
        return mAssignToken;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getValue() const {
        return mValue;
    }

    [[nodiscard]] LocalVariableSymbol* getSymbolRef() const {
        return mSymbolRef;
    }

    void setSymbolRef(LocalVariableSymbol* symbolRef) {
        mSymbolRef = symbolRef;
    }

   private:
    Token mVarKeyword;
    Token mType;
    Token mName;
    Token mAssignToken;
    std::unique_ptr<Expression> mValue;
    LocalVariableSymbol* mSymbolRef{nullptr};
};

class Return : public Statement {
   public:
    Return(Token returnKeyword,
           std::unique_ptr<Expression> returnedExpression,
           const SourceLocation& sourceLocation);

    ~Return() override;
    Return(const Return& other) = delete;
    Return(Return&& other) noexcept = default;
    Return& operator=(const Return& other) = delete;
    Return& operator=(Return&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getReturnKeyword() const {
        return mReturnKeyword;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getReturnExpression() const {
        return mReturnExpression;
    }

   private:
    Token mReturnKeyword;
    std::unique_ptr<Expression> mReturnExpression;
};

class While : public Statement {
   public:
    While(Token whileKeyword,
          Token leftBracket,
          Token rightBracket,
          std::unique_ptr<Expression> condition,
          std::unique_ptr<StatementsBlock> body,
          const SourceLocation& sourceLocation);

    ~While() override;
    While(const While& other) = delete;
    While(While&& other) noexcept = default;
    While& operator=(const While& other) = delete;
    While& operator=(While&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getWhileKeyword() const {
        return mWhileKeyword;
    }

    [[nodiscard]] const Token& getLeftBracket() const {
        return mLeftBracket;
    }

    [[nodiscard]] const Token& getRightBracket() const {
        return mRightBracket;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getCondition() const {
        return mCondition;
    }

    [[nodiscard]] const std::unique_ptr<StatementsBlock>& getBody() const {
        return mBody;
    }

   private:
    Token mWhileKeyword;
    Token mLeftBracket;
    Token mRightBracket;
    std::unique_ptr<Expression> mCondition;
    std::unique_ptr<StatementsBlock> mBody;
};

#endif  // VUG_STATEMENTS_HPP

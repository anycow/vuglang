// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_EXPRESSIONS_HPP
#define VUG_EXPRESSIONS_HPP

#include <memory>
#include <vector>

#include "AST/ASTWalker.hpp"
#include "AST/Node.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

class Type;

class Expression : public Node {
   public:
    Expression(Kind nodeType, const SourceLocation& sourceLocation);

    ~Expression() override = default;
    Expression(const Expression& other) = default;
    Expression(Expression&& other) noexcept = default;
    Expression& operator=(const Expression& other) = delete;
    Expression& operator=(Expression&& other) noexcept = delete;

    [[nodiscard]] bool isExpression() const override {
        return true;
    }

    [[nodiscard]] const Type* getExprType() const {
        return mExprType;
    }
    void setExprType(const Type* exprType) {
        mExprType = exprType;
    }

   private:
    const Type* mExprType{nullptr};
};

class BadExpression : public Expression {
   public:
    BadExpression();

    ~BadExpression() override;
    BadExpression(const BadExpression& other) = default;
    BadExpression(BadExpression&& other) noexcept = default;
    BadExpression& operator=(const BadExpression& other) = delete;
    BadExpression& operator=(BadExpression&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    [[nodiscard]] bool isInvalid() const override {
        return true;
    }
};

class BinaryOperation : public Expression {
   public:
    BinaryOperation(Token operation,
                    std::unique_ptr<Expression> left,
                    std::unique_ptr<Expression> right,
                    const SourceLocation& sourceLocation);

    ~BinaryOperation() override;
    BinaryOperation(const BinaryOperation& other) = delete;
    BinaryOperation(BinaryOperation&& other) noexcept = default;
    BinaryOperation& operator=(const BinaryOperation& other) = delete;
    BinaryOperation& operator=(BinaryOperation&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getOperation() const {
        return mOperation;
    }

    [[nodiscard]] LexemType getOperationType() const {
        return mOperation.getType();
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getLeft() const {
        return mLeft;
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getRight() const {
        return mRight;
    }

   private:
    Token mOperation;
    std::unique_ptr<Expression> mLeft;
    std::unique_ptr<Expression> mRight;
};

class CallFunction : public Expression {
   public:
    CallFunction(Token name,
                 Token leftBracket,
                 Token rightBracket,
                 std::vector<std::unique_ptr<Expression>> expressions,
                 const SourceLocation& sourceLocation);

    ~CallFunction() override;
    CallFunction(const CallFunction& other) = default;
    CallFunction(CallFunction&& other) noexcept = default;
    CallFunction& operator=(const CallFunction& other) = delete;
    CallFunction& operator=(CallFunction&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getName() const {
        return mName;
    }

    [[nodiscard]] const Token& getLeftBracket() const {
        return mLeftBracket;
    }

    [[nodiscard]] const Token& getRightBracket() const {
        return mRightBracket;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Expression>>& getArguments() const {
        return mArguments;
    }

    [[nodiscard]] FunctionSymbol* getSymbolRef() const {
        return mSymbolRef;
    }
    void setSymbolRef(FunctionSymbol* symbolRef) {
        mSymbolRef = symbolRef;
    }

   private:
    Token mName;
    Token mLeftBracket;
    Token mRightBracket;
    std::vector<std::unique_ptr<Expression>> mArguments;
    FunctionSymbol* mSymbolRef{nullptr};
};

class Identifier : public Expression {
   public:
    Identifier(Token name, const SourceLocation& sourceLocation);

    ~Identifier() override;
    Identifier(const Identifier& other) = default;
    Identifier(Identifier&& other) noexcept = default;
    Identifier& operator=(const Identifier& other) = delete;
    Identifier& operator=(Identifier&& other) noexcept = delete;

    [[nodiscard]] LocalVariableSymbol* getVariableSymbolPtr() const {
        return mSymbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getName() const {
        return mName;
    }

    [[nodiscard]] LocalVariableSymbol* getSymbolRef() const {
        return mSymbolRef;
    }

    void setSymbolRef(LocalVariableSymbol* symbolRef) {
        mSymbolRef = symbolRef;
    }

   private:
    Token mName;
    LocalVariableSymbol* mSymbolRef{nullptr};
};

class Number : public Expression {
   public:
    Number(Token number, const SourceLocation& sourceLocation);

    ~Number() override;
    Number(const Number& other) = default;
    Number(Number&& other) noexcept = default;
    Number& operator=(const Number& other) = delete;
    Number& operator=(Number&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getNumber() const {
        return mNumber;
    }

   private:
    Token mNumber;
};

class PrefixOperation : public Expression {
   public:
    PrefixOperation(Token operation,
                    std::unique_ptr<Expression> right,
                    const SourceLocation& sourceLocation);

    ~PrefixOperation() override;
    PrefixOperation(const PrefixOperation& other) = delete;
    PrefixOperation(PrefixOperation&& other) noexcept = default;
    PrefixOperation& operator=(const PrefixOperation& other) = delete;
    PrefixOperation& operator=(PrefixOperation&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getOperation() const {
        return mOperation;
    }

    [[nodiscard]] LexemType getOperationType() const {
        return mOperation.getType();
    }

    [[nodiscard]] const std::unique_ptr<Expression>& getRight() const {
        return mRight;
    }

   private:
    Token mOperation;
    std::unique_ptr<Expression> mRight;
};

#endif  // VUG_EXPRESSIONS_HPP

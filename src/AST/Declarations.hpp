// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_DECLARATIONS_HPP
#define VUG_DECLARATIONS_HPP

#include <memory>
#include <vector>

#include "AST/ASTWalker.hpp"
#include "AST/Node.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

class Expression;
class StatementsBlock;

class Declaration : public Node {
   public:
    Declaration(Kind nodeType, const SourceLocation& sourceLocation);

    ~Declaration() override = default;
    Declaration(const Declaration& other) = default;
    Declaration(Declaration&& other) noexcept = default;
    Declaration& operator=(const Declaration& other) = delete;
    Declaration& operator=(Declaration&& other) noexcept = delete;

    [[nodiscard]] bool isDeclaration() const override {
        return true;
    }

    [[nodiscard]] virtual Symbol* getSymbolPtr() const {
        return nullptr;
    }
};

class BadDeclaration : public Declaration {
   public:
    BadDeclaration();

    ~BadDeclaration() override;
    BadDeclaration(const BadDeclaration& other) = default;
    BadDeclaration(BadDeclaration&& other) noexcept = default;
    BadDeclaration& operator=(const BadDeclaration& other) = delete;
    BadDeclaration& operator=(BadDeclaration&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    [[nodiscard]] bool isInvalid() const override {
        return true;
    }
};

class DeclarationsBlock : public Declaration {
   public:
    DeclarationsBlock(Token leftBracket,
                      Token rightBracket,
                      std::vector<std::unique_ptr<Declaration>> declarations,
                      const SourceLocation& sourceLocation);

    ~DeclarationsBlock() override;
    DeclarationsBlock(const DeclarationsBlock& other) = default;
    DeclarationsBlock(DeclarationsBlock&& other) noexcept = default;
    DeclarationsBlock& operator=(const DeclarationsBlock& other) = delete;
    DeclarationsBlock& operator=(DeclarationsBlock&& other) noexcept = delete;

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getLeftBracket() const {
        return mLeftBracket;
    }

    [[nodiscard]] const Token& getRightBracket() const {
        return mRightBracket;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Declaration>>& getDeclarations() const {
        return mDeclarations;
    }

   private:
    Token mLeftBracket;
    Token mRightBracket;
    std::vector<std::unique_ptr<Declaration>> mDeclarations;
};

class FunctionParameter : public Declaration {
   public:
    FunctionParameter(Token type, Token name, const SourceLocation& sourceLocation);

    ~FunctionParameter() override;
    FunctionParameter(const FunctionParameter& other) = default;
    FunctionParameter(FunctionParameter&& other) noexcept = default;
    FunctionParameter& operator=(const FunctionParameter& other) = delete;
    FunctionParameter& operator=(FunctionParameter&& other) noexcept = delete;

    [[nodiscard]] LocalVariableSymbol* getSymbolPtr() const override {
        return mSymbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getType() const {
        return mType;
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
    Token mType;
    Token mName;
    LocalVariableSymbol* mSymbolRef{nullptr};
};

class FunctionDeclaration : public Declaration {
   public:
    FunctionDeclaration(Token funcKeyword,
                        Token name,
                        Token leftBracket,
                        Token rightBracket,
                        Token arrow,
                        Token returnType,
                        std::vector<std::unique_ptr<FunctionParameter>> parameters,
                        std::unique_ptr<StatementsBlock> definition,
                        const SourceLocation& sourceLocation);

    ~FunctionDeclaration() override;
    FunctionDeclaration(const FunctionDeclaration& other) = delete;
    FunctionDeclaration(FunctionDeclaration&& other) noexcept = default;
    FunctionDeclaration& operator=(const FunctionDeclaration& other) = delete;
    FunctionDeclaration& operator=(FunctionDeclaration&& other) noexcept = delete;

    [[nodiscard]] FunctionSymbol* getSymbolPtr() const override {
        return mSymbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getFuncKeyword() const {
        return mFuncKeyword;
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

    [[nodiscard]] const Token& getArrow() const {
        return mArrow;
    }

    [[nodiscard]] const Token& getReturnType() const {
        return mReturnType;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<FunctionParameter>>& getParameters() const {
        return mParameters;
    }

    [[nodiscard]] const std::unique_ptr<StatementsBlock>& getDefinition() const {
        return mDefinition;
    }

    [[nodiscard]] FunctionSymbol* getSymbolRef() const {
        return mSymbolRef;
    }
    void setSymbolRef(FunctionSymbol* symbolRef) {
        mSymbolRef = symbolRef;
    }

   private:
    Token mFuncKeyword;
    Token mName;
    Token mLeftBracket;
    Token mRightBracket;
    Token mArrow;
    Token mReturnType;
    std::vector<std::unique_ptr<FunctionParameter>> mParameters;
    std::unique_ptr<StatementsBlock> mDefinition;
    FunctionSymbol* mSymbolRef{nullptr};
};

class ModuleDeclaration : public Declaration {
   public:
    ModuleDeclaration(Token modKeyword,
                      Token name,
                      std::unique_ptr<DeclarationsBlock> body,
                      const SourceLocation& sourceLocation);

    ~ModuleDeclaration() override;
    ModuleDeclaration(const ModuleDeclaration& other) = delete;
    ModuleDeclaration(ModuleDeclaration&& other) noexcept = default;
    ModuleDeclaration& operator=(const ModuleDeclaration& other) = delete;
    ModuleDeclaration& operator=(ModuleDeclaration&& other) noexcept = delete;

    [[nodiscard]] ModuleSymbol* getSymbolPtr() const override {
        return mSymbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }

    [[nodiscard]] const Token& getModKeyword() const {
        return mModKeyword;
    }

    [[nodiscard]] const Token& getName() const {
        return mName;
    }

    [[nodiscard]] const std::unique_ptr<DeclarationsBlock>& getBody() const {
        return mBody;
    }

    [[nodiscard]] ModuleSymbol* getSymbolRef() const {
        return mSymbolRef;
    }
    void setSymbolRef(ModuleSymbol* symbolRef) {
        mSymbolRef = symbolRef;
    }

   private:
    Token mModKeyword;
    Token mName;
    std::unique_ptr<DeclarationsBlock> mBody;
    ModuleSymbol* mSymbolRef{nullptr};
};

#endif  // VUG_DECLARATIONS_HPP

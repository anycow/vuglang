// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_PARSER_HPP
#define VUG_PARSER_HPP

#include <memory>
#include <utility>

#include "AST/ASTNodesForward.hpp"
#include "Diagnostic/Diagnostic.hpp"
#include "Lexing/Lexer.hpp"

class DiagnosticManager;

class Parser {
   public:
    explicit Parser(Lexer& lexer, DiagnosticManager& diagnosticManager)
        : mLexer(lexer),
          mDiagnosticManager(diagnosticManager) {
        advance();
    }

    Token advance() {
        mPrevious = mCurrent;
        return (mCurrent = mLexer.getToken());
    }

    std::unique_ptr<Node> program();

    std::unique_ptr<Declaration> declaration();
    std::unique_ptr<ModuleDeclaration> moduleDeclaration();
    std::unique_ptr<FunctionDeclaration> functionDeclaration();
    std::unique_ptr<FunctionParameter> functionParameter();
    std::unique_ptr<DeclarationsBlock> declarationsBlock();

    std::unique_ptr<Statement> stmt();
    std::unique_ptr<If> ifStmt();
    std::unique_ptr<While> whileStmt();
    std::unique_ptr<Break> breakStmt();
    std::unique_ptr<Return> returnStmt();
    std::unique_ptr<LocalVariableDeclaration> localVariableDeclaration();
    std::unique_ptr<Statement> varAssign();
    std::unique_ptr<StatementsBlock> stmtBlock();

    std::unique_ptr<Expression> expr();
    std::unique_ptr<Expression> logicOr();
    std::unique_ptr<Expression> logicAnd();
    std::unique_ptr<Expression> bitOr();
    std::unique_ptr<Expression> bitXor();
    std::unique_ptr<Expression> bitAnd();
    std::unique_ptr<Expression> equal();
    std::unique_ptr<Expression> relat();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> fact();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();

   private:
    Lexer& mLexer;
    DiagnosticManager& mDiagnosticManager;

    Token mCurrent{LexemType::EndOfFile, SourceLocation()};
    Token mPrevious{LexemType::EndOfFile, SourceLocation()};

    uint32_t mLoopNestingDepth = 0;
};

class ParsingException : public std::exception {
   public:
    explicit ParsingException(Diagnostic diagnostic)
        : mDiagnostic(std::move(diagnostic)) {
    }

    [[nodiscard]] const char* what() const noexcept override {
        return "Uncaught ParsingException";
    }

    [[nodiscard]] const Diagnostic& getDiagnostic() const {
        return mDiagnostic;
    }

   private:
    const Diagnostic mDiagnostic;
};

#endif  // VUG_PARSER_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_PARSER_HPP
#define VUG_PARSER_HPP

#include <memory>

#include "AST/ASTNodesForward.hpp"
#include "Lexing/Lexer.hpp"

class Parser {
public:
    explicit Parser(Lexer& lexer) : _lexer(lexer) { advance(); }

    Token advance() {
        return (_current = _lexer.getToken());
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
    std::unique_ptr<LocalVariableDeclaration> localVarDeclaration();
    std::unique_ptr<Statement> varAssignOrCallStmt();
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

protected:
    Lexer& _lexer;
    Token _current;
    uint32_t loopNestingDepth = 0;
};

#endif//VUG_PARSER_HPP

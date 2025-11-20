// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_NODE_HPP
#define VUG_NODE_HPP

#include <memory>
#include <string>
#include <vector>

#include "AST/ASTNodesForward.hpp"
#include "AST/ASTWalker.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

struct Node {
    enum class Kind {
        None,

        BadNode,
        BadDeclaration,
        BadExpression,
        BadStatement,

        DeclarationsBlock,
        FunctionDeclaration,
        FunctionParameter,
        ModuleDeclaration,

        Assign,
        BinaryOperation,
        Identifier,
        Number,
        PrefixOperation,

        StatementBlock,
        Break,
        CallFunction,
        If,
        Print,
        Return,
        LocalVarDeclaration,
        While,
    };

    const Kind kind;
    const SourceLocation sourceLocation;

    virtual ~Node() = default;

    virtual void accept(ASTWalker& walker) { throw std::logic_error("Not implemented"); }

    virtual bool isExpression() { return false; }
    virtual bool isStatement() { return false; }
    virtual bool isDeclaration() { return false; }
    virtual bool isInvalid() { return false; }

protected:
    explicit Node(Kind nodeType, SourceLocation sourceLocation)
        : kind(nodeType),
          sourceLocation(sourceLocation) {}
};


#endif//VUG_NODE_HPP

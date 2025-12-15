// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_NODE_HPP
#define VUG_NODE_HPP

#include <stdexcept>

#include "AST/ASTWalker.hpp"
#include "Lexing/Token.hpp"

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
        CallFunction,
        Identifier,
        Number,
        PrefixOperation,

        StatementBlock,
        Break,
        ExpressionStatement,
        If,
        Return,
        LocalVarDeclaration,
        While,
    };

    const Kind kind;
    const SourceLocation sourceLocation;

    virtual ~Node() = default;
    Node(const Node& other) = default;
    Node(Node&& other) noexcept = default;
    Node& operator=(const Node& other) = delete;
    Node& operator=(Node&& other) noexcept = delete;

    virtual void accept([[maybe_unused]] ASTWalker& walker) {
        throw std::logic_error("Not implemented");
    }

    [[nodiscard]] virtual bool isExpression() const {
        return false;
    }
    [[nodiscard]] virtual bool isStatement() const {
        return false;
    }
    [[nodiscard]] virtual bool isDeclaration() const {
        return false;
    }
    [[nodiscard]] virtual bool isInvalid() const {
        return false;
    }

   protected:
    Node(const Kind nodeType, const SourceLocation& sourceLocation)
        : kind(nodeType),
          sourceLocation(sourceLocation) {
    }
};


#endif  // VUG_NODE_HPP

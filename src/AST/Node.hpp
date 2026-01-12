// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef NODE_HPP
#define NODE_HPP

#include <stdexcept>

#include "Lexing/Token.hpp"

class ASTWalker;
class Node {
   public:
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

    [[nodiscard]] Kind getKind() const {
        return mKind;
    }

    [[nodiscard]] const SourceLocation& getSourceLocation() const {
        return mSourceLocation;
    }

   protected:
    Node(const Kind nodeType, const SourceLocation& sourceLocation)
        : mKind(nodeType),
          mSourceLocation(sourceLocation) {
    }

   private:
    /// Explicit type tag used instead of RTTI or virtual methods for fast dispatch.
    const Kind mKind;
    const SourceLocation mSourceLocation;
};


#endif  // NODE_HPP

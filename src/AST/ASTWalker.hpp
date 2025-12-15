// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_ASTWALKER_HPP
#define VUG_ASTWALKER_HPP

#include <stdexcept>

#include "AST/ASTNodesForward.hpp"

class ASTWalker {
   public:
    ASTWalker() = default;
    virtual ~ASTWalker() = default;
    ASTWalker(const ASTWalker& other) = default;
    ASTWalker(ASTWalker&& other) noexcept = default;
    ASTWalker& operator=(const ASTWalker& other) = default;
    ASTWalker& operator=(ASTWalker&& other) noexcept = default;

    virtual void visit([[maybe_unused]] BadDeclaration& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] BadExpression& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] BadStatement& node) {
        throw std::logic_error("Not implemented");
    }

    virtual void visit([[maybe_unused]] DeclarationsBlock& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] FunctionDeclaration& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] FunctionParameter& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] ModuleDeclaration& node) {
        throw std::logic_error("Not implemented");
    }

    virtual void visit([[maybe_unused]] Number& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] Identifier& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] BinaryOperation& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] PrefixOperation& node) {
        throw std::logic_error("Not implemented");
    }

    virtual void visit([[maybe_unused]] Assign& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] Break& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] ExpressionStatement& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] CallFunction& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] If& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] LocalVariableDeclaration& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] Return& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] StatementsBlock& node) {
        throw std::logic_error("Not implemented");
    }
    virtual void visit([[maybe_unused]] While& node) {
        throw std::logic_error("Not implemented");
    }

   protected:
    virtual void visit([[maybe_unused]] Node& node) {
        throw std::logic_error("Not implemented");
    }
};

#endif  // VUG_ASTWALKER_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_ASTWALKER_HPP
#define VUG_ASTWALKER_HPP

#include "AST/ASTNodesForward.hpp"

class ASTWalker {
public:
    virtual ~ASTWalker() = default;

    virtual void visit(BadDeclaration& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(BadExpression& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(BadStatement& node) { throw std::logic_error("Not implemented"); }

    virtual void visit(DeclarationsBlock& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(FunctionDeclaration& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(FunctionParameter& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(ModuleDeclaration& node) { throw std::logic_error("Not implemented"); }

    virtual void visit(Number& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(Identifier& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(BinaryOperation& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(PrefixOperation& node) { throw std::logic_error("Not implemented"); }

    virtual void visit(Assign& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(Break& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(CallFunction& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(If& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(LocalVariableDeclaration& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(Print& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(Return& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(StatementsBlock& node) { throw std::logic_error("Not implemented"); }
    virtual void visit(While& node) { throw std::logic_error("Not implemented"); }

protected:
    virtual void visit(Node& node) { throw std::logic_error("Not implemented"); }
};

#endif//VUG_ASTWALKER_HPP

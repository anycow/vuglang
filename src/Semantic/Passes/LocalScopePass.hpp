// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_LOCALSCOPEPASS_HPP
#define VUG_LOCALSCOPEPASS_HPP

#include <stack>

#include "AST/ASTWalker.hpp"

class SymbolContext;

class LocalScopePass : public ASTWalker {
public:
    LocalScopePass(Node& ast,
                   SymbolContext& context) : _ast(ast), _context(context) {}

    void analyze();

    void visit(ModuleDeclaration& node) override;
    void visit(DeclarationsBlock& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(FunctionParameter& node) override;

    void visit(CallFunction& node) override;
    void visit(Number& node) override;
    void visit(Identifier& node) override;
    void visit(BinaryOperation& node) override;
    void visit(PrefixOperation& node) override;

    void visit(Assign& node) override;
    void visit(LocalVariableDeclaration& node) override;
    void visit(StatementsBlock& node) override;
    void visit(Break& node) override;
    void visit(If& node) override;
    void visit(While& node) override;
    void visit(Print& node) override;
    void visit(Return& node) override;

protected:
    Node& _ast;
    SymbolContext& _context;

    std::stack<While*> _loops;
    FunctionDeclaration* _currentFunction{nullptr};

    void visit(Node& node) override;
};

#endif//VUG_LOCALSCOPEPASS_HPP

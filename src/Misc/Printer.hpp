// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_PRINTER_HPP
#define VUG_PRINTER_HPP

#include "AST/ASTNodes.hpp"
#include "AST/ASTWalker.hpp"

class Printer : public ASTWalker {
   public:
    Printer(Node& ast, const uint32_t tabSize)
        : _ast(ast),
          _tabSize(tabSize) {
    }

    void print();

    void visit(BadDeclaration& node) override;
    void visit(BadExpression& node) override;
    void visit(BadStatement& node) override;

    void visit(DeclarationsBlock& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(FunctionParameter& node) override;
    void visit(ModuleDeclaration& node) override;

    void visit(Number& node) override;
    void visit(Identifier& node) override;
    void visit(BinaryOperation& node) override;
    void visit(PrefixOperation& node) override;

    void visit(Assign& node) override;
    void visit(Break& node) override;
    void visit(If& node) override;
    void visit(LocalVariableDeclaration& node) override;
    void visit(Print& node) override;
    void visit(Return& node) override;
    void visit(StatementsBlock& node) override;
    void visit(While& node) override;
    void visit(CallFunction& node) override;

   protected:
    Node& _ast;
    uint32_t _currentDepth = 0;
    uint32_t _tabSize = 0;

    void visit(Node& node) override;
    [[nodiscard]] std::string getIndentSpaces() const;
};

#endif  // VUG_PRINTER_HPP

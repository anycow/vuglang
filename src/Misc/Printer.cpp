// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "Printer.hpp"

#include <iostream>
#include <string>

#include "AST/Declarations.hpp"
#include "AST/Expressions.hpp"
#include "AST/Node.hpp"
#include "AST/Statements.hpp"
#include "Lexing/Token.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/Types/Type.hpp"

std::string Printer::getIndentSpaces() const {
    return std::string(
        static_cast<std::basic_string<char>::size_type>((mCurrentDepth - 1) * mTabSize),
        '-');
}

void Printer::print() {
    stackGuard();

    mAst.accept(*this);
}
void Printer::visit(Node& node) {
    stackGuard();

    node.accept(*this);
}

void Printer::visit([[maybe_unused]] BadDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Bad Declaration: " << std::endl;

    --mCurrentDepth;
}
void Printer::visit([[maybe_unused]] BadExpression& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Bad Expression:" << std::endl;

    --mCurrentDepth;
}
void Printer::visit([[maybe_unused]] BadStatement& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Bad Statement:" << std::endl;

    --mCurrentDepth;
}

void Printer::visit(FunctionDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << "Function Declaration: "
        << node.getReturnType().getValue()
        << ' '
        << node.getName().getValue()
        << std::endl;

    for (const auto& parameter : node.getParameters()) {
        visit(*parameter);
    }

    if (node.getDefinition()) {
        visit(*node.getDefinition());
    }

    --mCurrentDepth;
}
void Printer::visit(FunctionParameter& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << "Function Parameter: "
        << node.getType().getValue()
        << ' '
        << node.getName().getValue()
        << std::endl;

    --mCurrentDepth;
}
void Printer::visit(ModuleDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << "Module Declaration: "
        << node.getName().getValue()
        << std::endl;

    visit(*node.getBody());

    --mCurrentDepth;
}
void Printer::visit(DeclarationsBlock& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Declarations Block: " << std::endl;
    for (const auto& declaration : node.getDeclarations()) {
        visit(*declaration);
    }

    --mCurrentDepth;
}

void Printer::visit(Assign& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Assign: " << node.getName().getValue() << std::endl;

    visit(*node.getValue());

    --mCurrentDepth;
}
void Printer::visit(Number& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << (node.getExprType() != nullptr ? "(" + node.getExprType()->getTypeName() + ")" : "")
        << "Number: "
        << node.getNumber().getValue()
        << std::endl;

    --mCurrentDepth;
}
void Printer::visit(Identifier& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << (node.getExprType() != nullptr ? "(" + node.getExprType()->getTypeName() + ")" : "")
        << "Identifier: "
        << node.getName().getValue()
        << std::endl;

    --mCurrentDepth;
}
void Printer::visit(BinaryOperation& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << (node.getExprType() != nullptr ? "(" + node.getExprType()->getTypeName() + ")" : "")
        << "BinOp: "
        << TokenTypeNames[node.getOperationType()]
        << std::endl;

    visit(*node.getLeft());
    visit(*node.getRight());

    --mCurrentDepth;
}
void Printer::visit(PrefixOperation& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << (node.getExprType() != nullptr ? "(" + node.getExprType()->getTypeName() + ")" : "")
        << "PrefixOp: "
        << TokenTypeNames[node.getOperationType()]
        << std::endl;

    visit(*node.getRight());

    --mCurrentDepth;
}
void Printer::visit(LocalVariableDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout
        << getIndentSpaces()
        << "Var Declaration: "
        << node.getType().getValue()
        << " "
        << node.getName().getValue()
        << std::endl;

    if (node.getValue() != nullptr) {
        visit(*node.getValue());
    }

    --mCurrentDepth;
}
void Printer::visit(StatementsBlock& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Block: " << std::endl;

    for (const auto& item : node.getStatements()) {
        visit(*item);
    }

    --mCurrentDepth;
}
void Printer::visit([[maybe_unused]] Break& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Break: " << std::endl;

    --mCurrentDepth;
}
void Printer::visit(CallFunction& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Call: " << node.getName().getValue() << std::endl;

    for (const auto& argument : node.getArguments()) {
        visit(*argument);
    }

    --mCurrentDepth;
}
void Printer::visit(If& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "If: " << std::endl;

    visit(*node.getCondition());
    visit(*node.getThen());

    if (node.getElseThen() != nullptr) {
        visit(*node.getElseThen());
    }

    --mCurrentDepth;
}
void Printer::visit(While& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "While: " << std::endl;

    visit(*node.getCondition());
    visit(*node.getBody());

    --mCurrentDepth;
}
void Printer::visit(ExpressionStatement& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Expression Statement: " << std::endl;

    visit(*node.getExpression());

    --mCurrentDepth;
}
void Printer::visit(Return& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Return: " << std::endl;

    visit(*node.getReturnExpression());

    --mCurrentDepth;
}

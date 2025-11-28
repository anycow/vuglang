// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "Printer.hpp"

#include <iostream>

#include "AST/ASTNodes.hpp"
#include "Misc/Stack.hpp"

std::string Printer::getIndentSpaces() const {
    return std::string((mCurrentDepth - 1) * mTabSize, '-');
}

void Printer::print() {
    stackGuard();

    mAst.accept(*this);
}
void Printer::visit(Node& node) {
    stackGuard();

    node.accept(*this);
}

void Printer::visit(BadDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Bad Declaration: " << std::endl;

    --mCurrentDepth;
}
void Printer::visit(BadExpression& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Bad Expression:" << std::endl;

    --mCurrentDepth;
}
void Printer::visit(BadStatement& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Bad Statement:" << std::endl;

    --mCurrentDepth;
}

void Printer::visit(FunctionDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Function Declaration: " << node.returnType << ' '
              << node.name << std::endl;

    for (const auto& parameter : node.parameters) {
        visit(*parameter);
    }

    if (node.definition) {
        visit(*node.definition);
    }

    --mCurrentDepth;
}
void Printer::visit(FunctionParameter& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Function Parameter: " << node.type << ' ' << node.name
              << std::endl;

    --mCurrentDepth;
}
void Printer::visit(ModuleDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Module Declaration: " << node.name << std::endl;

    visit(*node.body);

    --mCurrentDepth;
}
void Printer::visit(DeclarationsBlock& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Declarations Block: " << std::endl;
    for (const auto& declaration : node.declarations) {
        visit(*declaration);
    }

    --mCurrentDepth;
}

void Printer::visit(Assign& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Assign: " << node.name << std::endl;

    visit(*node.value);

    --mCurrentDepth;
}
void Printer::visit(Number& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces()
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "Number: " << node.number << std::endl;

    --mCurrentDepth;
}
void Printer::visit(Identifier& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces()
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "Identifier: " << node.name << std::endl;

    --mCurrentDepth;
}
void Printer::visit(BinaryOperation& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces()
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "BinOp: " << TokenTypeNames[node.operationType] << std::endl;

    visit(*node.left);
    visit(*node.right);

    --mCurrentDepth;
}
void Printer::visit(PrefixOperation& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces()
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "PrefixOp: " << TokenTypeNames[node.operationType] << std::endl;

    visit(*node.right);

    --mCurrentDepth;
}
void Printer::visit(LocalVariableDeclaration& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Var Declaration: " << node.type << " " << node.name
              << std::endl;

    if (node.value != nullptr) {
        visit(*node.value);
    }

    --mCurrentDepth;
}
void Printer::visit(StatementsBlock& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Block: " << std::endl;

    for (const auto& item : node.statements) {
        visit(*item);
    }

    --mCurrentDepth;
}
void Printer::visit(Break& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Break: " << std::endl;

    --mCurrentDepth;
}
void Printer::visit(CallFunction& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Call: " << node.name << std::endl;

    for (const auto& argument : node.arguments) {
        visit(*argument);
    }

    --mCurrentDepth;
}
void Printer::visit(If& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "If: " << std::endl;

    visit(*node.condition);
    visit(*node.then);

    if (node.elseThen != nullptr) {
        visit(*node.elseThen);
    }

    --mCurrentDepth;
}
void Printer::visit(While& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "While: " << std::endl;

    visit(*node.condition);
    visit(*node.body);

    --mCurrentDepth;
}
void Printer::visit(Print& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Print: " << std::endl;

    visit(*node.expression);

    --mCurrentDepth;
}
void Printer::visit(ExpressionStatement& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Expression Statement: " << std::endl;

    visit(*node.expression);

    --mCurrentDepth;
}
void Printer::visit(Return& node) {
    stackGuard();
    ++mCurrentDepth;

    std::cout << getIndentSpaces() << "Return: " << std::endl;

    visit(*node.returnExpression);

    --mCurrentDepth;
}

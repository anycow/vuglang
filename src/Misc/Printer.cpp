// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Printer.hpp"

#include "AST/ASTNodes.hpp"
#include "Misc/Stack.hpp"

void Printer::print() {
    stackGuard();

    _ast.accept(*this);
}
void Printer::visit(Node& node) {
    stackGuard();

    node.accept(*this);
}

void Printer::visit(FunctionDeclaration& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces
              << "Function Declaration: "
              << node.returnType << ' '
              << node.name << std::endl;

    for (const auto& parameter: node.parameters) {
        visit(*parameter);
    }

    visit(*node.definition);

    --_currentDepth;
}
void Printer::visit(FunctionParameter& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Function Parameter: "
              << node.type << ' '
              << node.name << std::endl;

    --_currentDepth;
}
void Printer::visit(ModuleDeclaration& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Module Declaration: " << node.name << std::endl;

    visit(*node.body);

    --_currentDepth;
}
void Printer::visit(DeclarationsBlock& node) {
    stackGuard();
    ++_currentDepth;

    for (const auto& declaration: node.declarations) {
        visit(*declaration);
    }

    --_currentDepth;
}

void Printer::visit(Assign& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Assign: " << node.name << std::endl;

    visit(*node.value);

    --_currentDepth;
}
void Printer::visit(Number& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "Number: "
              << node.number
              << std::endl;

    --_currentDepth;
}
void Printer::visit(Identifier& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "Identifier: "
              << node.name
              << std::endl;

    --_currentDepth;
}
void Printer::visit(BinaryOperation& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "BinOp: "
              << TokenTypeNames[node.operationType]
              << std::endl;

    visit(*node.left);
    visit(*node.right);

    --_currentDepth;
}
void Printer::visit(PrefixOperation& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces
              << (node.exprType != nullptr ? "(" + node.exprType->getTypeName() + ")" : "")
              << "PrefixOp: "
              << TokenTypeNames[node.operationType]
              << std::endl;

    visit(*node.right);

    --_currentDepth;
}
void Printer::visit(LocalVariableDeclaration& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Var Declaration: " << node.type << " " << node.name << std::endl;

    if (node.value != nullptr) {
        visit(*node.value);
    }

    --_currentDepth;
}
void Printer::visit(StatementsBlock& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Block: " << std::endl;

    for (const auto& item: node.statements) {
        visit(*item);
    }

    --_currentDepth;
}
void Printer::visit(Break& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Break: " << std::endl;

    --_currentDepth;
}
void Printer::visit(CallFunction& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Call: " << node.name << std::endl;

    for (const auto& argument: node.arguments) {
        visit(*argument);
    }

    --_currentDepth;
}
void Printer::visit(If& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "If: " << std::endl;

    visit(*node.condition);
    visit(*node.then);

    if (node.elseThen != nullptr) {
        visit(*node.elseThen);
    }

    --_currentDepth;
}
void Printer::visit(While& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "While: " << std::endl;

    visit(*node.condition);
    visit(*node.body);

    --_currentDepth;
}
void Printer::visit(Print& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Print: " << std::endl;

    visit(*node.expression);

    --_currentDepth;
}
void Printer::visit(Return& node) {
    stackGuard();
    ++_currentDepth;

    std::string spaces((_currentDepth - 1) * _tabSize, '-');

    std::cout << spaces << "Return: " << std::endl;

    visit(*node.returnExpression);

    --_currentDepth;
}
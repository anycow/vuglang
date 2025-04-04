// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "LocalScopePass.hpp"

#include "AST/ASTNodes.hpp"
#include "Misc/Stack.hpp"
#include "Semantic/SymbolContext.hpp"
#include "Semantic/SymbolTable.hpp"
#include "Semantic/Type.hpp"

void LocalScopePass::analyze() {
    stackGuard();

    visit(_ast);
}
void LocalScopePass::visit(Node& node) {
    stackGuard();

    node.accept(*this);
}

void LocalScopePass::visit(ModuleDeclaration& node) {
    stackGuard();

    _context.getSymbolTable().openScope();
    auto result = _context.getSymbolTable().insertSymbol(*node.symbolRef);
    if (result.kind != SymbolTable::InsertResult::Kind::Successful) {
        throw std::logic_error("Name conflict");
    }
    visit(*node.body);
    _context.getSymbolTable().closeScope();
}
void LocalScopePass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (auto& declaration: node.declarations) {
        _context.getSymbolTable().insertSymbol(*declaration->getSymbolPtr());
    }
    for (auto& declaration: node.declarations) {
        visit(*declaration);
    }
}
void LocalScopePass::visit(FunctionDeclaration& node) {
    stackGuard();

    _currentFunction = &node;

    _context.getSymbolTable().openScope();
    for (const auto& parameter: node.parameters) {
        visit(*parameter);
    }
    visit(*node.definition);
    _context.getSymbolTable().closeScope();

    _currentFunction = nullptr;
}
void LocalScopePass::visit(FunctionParameter& node) {
    stackGuard();

    _context.getSymbolTable().insertSymbol(*node.symbolRef);
}

void LocalScopePass::visit(Assign& node) {
    stackGuard();

    visit(*node.value);

    auto result = _context.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        throw std::logic_error("Undeclared var");
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Variable) {
        throw std::logic_error("Symbol is not a variable");
    }
    node.symbolRef = static_cast<LocalVariableSymbol*>(&result.record->symbol);

    if ((*node.symbolRef->getTypeSymbol()->getType()) != (*node.value->exprType)) {
        throw std::logic_error("Incompatible types");
    }
}

void LocalScopePass::visit(CallFunction& node) {
    stackGuard();

    auto result = _context.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        throw std::logic_error("Undeclared var");
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Function) {
        throw std::logic_error("Symbol is not a function");
    }

    auto& functionSymbol = static_cast<FunctionSymbol&>(result.record->symbol);

    node.symbolRef = &functionSymbol;
    node.exprType = functionSymbol.getTypeSymbol()->getType();

    if (node.arguments.size() != functionSymbol.getArguments().size()) {
        throw std::logic_error("Different arguments count");
    }

    size_t index = 0;
    for (const auto& argument: node.arguments) {
        visit(*argument);
        if (argument->exprType != functionSymbol.getArguments()[index]->getTypeSymbol()->getType()) {
            throw std::logic_error("Incompatible types");
        }
        ++index;
    }
}
void LocalScopePass::visit(Number& node) {
    stackGuard();

    node.exprType = _context.getIntType(32, true)->getType();
}
void LocalScopePass::visit(Identifier& node) {
    stackGuard();

    auto result = _context.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        throw std::logic_error("Undeclared var");
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Variable) {
        throw std::logic_error("Symbol is not a variable");
    }

    node.symbolRef = static_cast<LocalVariableSymbol*>(&result.record->symbol);
    node.exprType = node.symbolRef->getTypeSymbol()->getType();
}
void LocalScopePass::visit(BinaryOperation& node) {
    stackGuard();

    visit(*node.left);
    visit(*node.right);

    auto checkResult =
            node.left->exprType->binaryOperationType(node.operationType, *node.right->exprType);

    if (checkResult.isTypesCorrect) {
        node.exprType = checkResult.resultType;
    } else {
        throw std::logic_error("Incompatible types");
    }
}

void LocalScopePass::visit(PrefixOperation& node) {
    stackGuard();

    visit(*node.right);

    auto checkResult =
            node.right->exprType->prefixOperationType(node.operationType);

    if (checkResult.isTypesCorrect) {
        node.exprType = checkResult.resultType;
    } else {
        throw std::logic_error("Incompatible types");
    }
}
void LocalScopePass::visit(LocalVariableDeclaration& node) {
    stackGuard();

    auto typeFindResult = _context.getSymbolTable().findSymbol(node.type);

    if (typeFindResult.kind != SymbolTable::FindResult::Kind::Successful) {
        throw std::logic_error("Undeclared type");
    }
    if (typeFindResult.record->symbol.getKind() != Symbol::Kind::Type) {
        throw std::logic_error("Symbol isn't type");
    }

    auto symbol = _context.addSymbol<LocalVariableSymbol>(node.name);
    symbol->setTypeSymbol(static_cast<TypeSymbol*>(&typeFindResult.record->symbol));
    node.symbolRef = symbol;

    auto insertResult = _context.getSymbolTable().insertSymbol(*node.symbolRef);
    if (insertResult.kind != SymbolTable::InsertResult::Kind::Successful) {
        throw std::logic_error("Name conflict");
    }

    if (node.value != nullptr) {
        visit(*node.value);

        if (*node.value->exprType != *node.symbolRef->getTypeSymbol()->getType()) {
            throw std::logic_error("Incompatible types");
        }
    }
}
void LocalScopePass::visit(StatementsBlock& node) {
    stackGuard();

    for (const auto& stmt: node.statements) {
        if (stmt->kind == Node::Kind::StatementBlock) {
            _context.getSymbolTable().openScope();
            visit(*stmt);
            _context.getSymbolTable().closeScope();
        } else {
            visit(*stmt);
        }
    }
}
void LocalScopePass::visit(Break& node) {
    stackGuard();

    node.breakedStmt = _loops.top();
}
void LocalScopePass::visit(If& node) {
    stackGuard();

    visit(*node.condition);
    visit(*node.then);
    if (node.elseThen != nullptr) {
        visit(*node.elseThen);
    }
}
void LocalScopePass::visit(While& node) {
    stackGuard();

    _loops.push(&node);
    _context.getSymbolTable().openScope();

    visit(*node.condition);
    for (const auto& stmt: node.body->statements) {
        visit(*stmt);
    }

    _loops.pop();
    _context.getSymbolTable().closeScope();
}
void LocalScopePass::visit(Print& node) {
    stackGuard();

    visit(*node.expression);
}
void LocalScopePass::visit(Return& node) {
    stackGuard();

    visit(*node.returnExpression);
    if (node.returnExpression->exprType != _currentFunction->symbolRef->getTypeSymbol()->getType()) {
        throw std::logic_error("Bad return type");
    }
}

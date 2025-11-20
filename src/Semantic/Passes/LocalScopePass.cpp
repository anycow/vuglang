// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "LocalScopePass.hpp"

#include "AST/ASTNodes.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
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

    if (!node.isInvalid()) {
        node.accept(*this);
    }
}

void LocalScopePass::visit(ModuleDeclaration& node) {
    stackGuard();

    _context.getSymbolTable().openScope();
    auto result = _context.getSymbolTable().insertSymbol(*node.symbolRef);
    if (result.kind != SymbolTable::InsertResult::Kind::Successful) {
        if (result.kind == SymbolTable::InsertResult::Kind::NameConflict) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("redefinition of '{}'", node.name),
                                                    {node.sourceLocation}));
            _diagnosticManager.report(diagnostic);
            return;
        } else if (result.kind == SymbolTable::InsertResult::Kind::ProhibitedShadowing) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("shadowing of '{}' is prohibited", node.name),
                                                    {node.sourceLocation}));
            _diagnosticManager.report(diagnostic);
            return;
        }
    }
    visit(*node.body);
    _context.getSymbolTable().closeScope();
}
void LocalScopePass::visit(DeclarationsBlock& node) {
    stackGuard();

    for (auto& declaration: node.declarations) {
        if (!declaration->isInvalid() && declaration->getSymbolPtr()) {
            _context.getSymbolTable().insertSymbol(*declaration->getSymbolPtr());
        }
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
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("can't find '{}'", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Variable) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't variable", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
    node.symbolRef = static_cast<LocalVariableSymbol*>(&result.record->symbol);

    if (*node.symbolRef->getTypeSymbol()->getType() != *node.value->exprType) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
}

void LocalScopePass::visit(CallFunction& node) {
    stackGuard();

    auto result = _context.getSymbolTable().findSymbol(node.name);

    if (result.kind != SymbolTable::FindResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared function '{}'", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Function) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't function", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }

    auto& functionSymbol = static_cast<FunctionSymbol&>(result.record->symbol);

    node.symbolRef = &functionSymbol;
    node.exprType = functionSymbol.getTypeSymbol()->getType();

    if (node.arguments.size() != functionSymbol.getArguments().size()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("different argument count", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }

    size_t index = 0;
    for (const auto& argument: node.arguments) {
        visit(*argument);
        if (argument->exprType != functionSymbol.getArguments()[index]->getTypeSymbol()->getType()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("incompatible types of arguments", node.name),
                                                    {node.sourceLocation}));
            _diagnosticManager.report(diagnostic);
            return;
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
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared variable '{}'", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
    if (result.record->symbol.getKind() != Symbol::Kind::Variable) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("'{}' isn't variable", node.name),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }

    node.symbolRef = static_cast<LocalVariableSymbol*>(&result.record->symbol);
    node.exprType = node.symbolRef->getTypeSymbol()->getType();
}
void LocalScopePass::visit(BinaryOperation& node) {
    stackGuard();

    visit(*node.left);
    visit(*node.right);

    auto checkResult =
            node.left->exprType->binaryOperationType(node.operationToken, *node.right->exprType);

    if (checkResult.isTypesCorrect) {
        node.exprType = checkResult.resultType;
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
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
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("incompatible types"),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
}
void LocalScopePass::visit(LocalVariableDeclaration& node) {
    stackGuard();

    auto typeFindResult = _context.getSymbolTable().findSymbol(node.type);

    if (typeFindResult.kind != SymbolTable::FindResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared type {}", node.type),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
    if (typeFindResult.record->symbol.getKind() != Symbol::Kind::Type) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("{} isn't type", node.type),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }

    auto symbol = _context.addSymbol<LocalVariableSymbol>(node.name);
    symbol->setTypeSymbol(static_cast<TypeSymbol*>(&typeFindResult.record->symbol));
    node.symbolRef = symbol;

    auto insertResult = _context.getSymbolTable().insertSymbol(*node.symbolRef);
    if (insertResult.kind != SymbolTable::InsertResult::Kind::Successful) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("undeclared type {}", node.type),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }

    if (node.value != nullptr) {
        visit(*node.value);

        if (*node.value->exprType != *node.symbolRef->getTypeSymbol()->getType()) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    std::format("incompatible types"),
                                                    {node.sourceLocation}));
            _diagnosticManager.report(diagnostic);
            return;
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

    if (_loops.empty()) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("break outisde loop"),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
    node.breakedStmt = _loops.top();
}
void LocalScopePass::visit(If& node) {
    stackGuard();

    visit(*node.condition);

    _context.getSymbolTable().openScope();
    visit(*node.then);
    _context.getSymbolTable().closeScope();

    if (node.elseThen != nullptr) {
        _context.getSymbolTable().openScope();
        visit(*node.elseThen);
        _context.getSymbolTable().closeScope();
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
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                std::format("bad return type"),
                                                {node.sourceLocation}));
        _diagnosticManager.report(diagnostic);
        return;
    }
}

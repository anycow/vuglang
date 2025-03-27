// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_EVALUATOR_HPP
#define VUG_EVALUATOR_HPP

#include <unordered_map>

#include "AST/ASTNodesForward.hpp"
#include "Evaluator/Objects/Object.hpp"

class Symbol;
class FunctionSymbol;
class SymbolContext;

enum class StmtResultKind {
    Successful,
    Break,
    Return,
};

struct StmtResult {
    StmtResultKind resultType;
    Statement* breakedStmt = nullptr;
    std::unique_ptr<Object> returnedObject = nullptr;

    StmtResult(StmtResultKind resultType)
        : resultType(resultType) {}

    StmtResult(Statement* breakedStmt)
        : resultType(StmtResultKind::Break),
          breakedStmt(breakedStmt) {}

    StmtResult(std::unique_ptr<Object> returnedObject)
        : resultType(StmtResultKind::Return),
          returnedObject(std::move(returnedObject)) {}
};

class Evaluator {
public:
    explicit Evaluator(Node& ast,
                       const SymbolContext& typeContext)
        : _ast(ast),
          _typeContext(typeContext) {}

    void evaluate();

    void evaluateDeclaration(const DeclarationsBlock& node);
    void evaluateDeclaration(const FunctionDeclaration& node);
    void evaluateDeclaration(const FunctionParameter& node);
    void evaluateDeclaration(const ModuleDeclaration& node);

    StmtResult evaluateStatement(const Assign& node);
    StmtResult evaluateStatement(const Break& node);
    StmtResult evaluateStatement(const If& node);
    StmtResult evaluateStatement(const LocalVariableDeclaration& node);
    StmtResult evaluateStatement(const Print& node);
    StmtResult evaluateStatement(const Return& node);
    StmtResult evaluateStatement(const StatementsBlock& node);
    StmtResult evaluateStatement(const While& node);

    std::unique_ptr<Object> evaluateExpression(const CallFunction& node);
    std::unique_ptr<Object> evaluateExpression(const Identifier& node);
    std::unique_ptr<Object> evaluateExpression(const Number& node);
    std::unique_ptr<Object> evaluateExpression(const BinaryOperation& node);
    std::unique_ptr<Object> evaluateExpression(const PrefixOperation& node);

protected:
    Node& _ast;
    const SymbolContext& _typeContext;
    std::unordered_map<const Symbol*, std::unique_ptr<Object>> _objects;

    StmtResult evaluateStatement(Statement& node);
    std::unique_ptr<Object> evaluateExpression(Expression& node);

    std::unique_ptr<Object> callFunction(const FunctionSymbol& functionSymbol, std::vector<std::unique_ptr<Object>> arguments);
};


#endif//VUG_EVALUATOR_HPP

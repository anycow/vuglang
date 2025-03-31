// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Evaluator.hpp"

#include "AST/ASTNodes.hpp"
#include "Evaluator/Objects/BooleanObject.hpp"
#include "Evaluator/Objects/IntegerObject.hpp"
#include "Misc/Stack.hpp"


void Evaluator::evaluate() {
    stackGuard();

    auto& mainSymbol = *static_cast<ModuleDeclaration&>(_ast).symbolRef->findMember("main")[0];
    callFunction(static_cast<FunctionSymbol&>(mainSymbol), {});
}


StmtResult Evaluator::evaluateStatement(Statement& node) {
    stackGuard();

    return node.evaluate(*this);
}
std::unique_ptr<Object> Evaluator::evaluateExpression(Expression& node) {
    stackGuard();

    return node.evaluate(*this);
}


void Evaluator::evaluateDeclaration(const DeclarationsBlock& node) {
    throw std::logic_error("not implemented");
}
void Evaluator::evaluateDeclaration(const FunctionDeclaration& node) {
    throw std::logic_error("not implemented");
}
void Evaluator::evaluateDeclaration(const FunctionParameter& node) {
    throw std::logic_error("not implemented");
}
void Evaluator::evaluateDeclaration(const ModuleDeclaration& node) {
    throw std::logic_error("not implemented");
}


StmtResult Evaluator::evaluateStatement(const Assign& node) {
    stackGuard();

    auto value = evaluateExpression(*node.value);
    _objects[node.symbolRef] = std::move(value);

    return {StmtResultKind::Successful};
}
StmtResult Evaluator::evaluateStatement(const Break& node) {
    stackGuard();

    return {node.breakedStmt};
}
StmtResult Evaluator::evaluateStatement(const If& node) {
    stackGuard();

    if (static_cast<BooleanObject&>(*evaluateExpression(*node.condition)).getValue()) {
        auto result = evaluateStatement(*node.then);
        if (result.resultType != StmtResultKind::Successful) {
            return result;
        }
    } else if (node.elseThen != nullptr) {
        auto result = evaluateStatement(*node.elseThen);
        if (result.resultType != StmtResultKind::Successful) {
            return result;
        }
    }

    return {StmtResultKind::Successful};
}
StmtResult Evaluator::evaluateStatement(const LocalVariableDeclaration& node) {
    stackGuard();

    auto value = evaluateExpression(*node.value);

    _objects[node.symbolRef] = std::move(value);

    return {StmtResultKind::Successful};
}
StmtResult Evaluator::evaluateStatement(const Print& node) {
    stackGuard();
    std::cout << evaluateExpression(*node.expression)->toString() << std::endl;

    return {StmtResultKind::Successful};
}
StmtResult Evaluator::evaluateStatement(const Return& node) {
    stackGuard();

    return {evaluateExpression(*node.returnExpression)};
}
StmtResult Evaluator::evaluateStatement(const StatementsBlock& node) {
    stackGuard();

    for (const auto& stmt: node.statements) {
        auto result = evaluateStatement(*stmt);
        if (result.resultType != StmtResultKind::Successful) {
            return result;
        }
    }

    return {StmtResultKind::Successful};
}
StmtResult Evaluator::evaluateStatement(const While& node) {
    stackGuard();

    while (static_cast<BooleanObject&>(*evaluateExpression(*node.condition)).getValue()) {
        auto result = evaluateStatement(*node.body);
        if (result.resultType != StmtResultKind::Successful) {
            if (result.breakedStmt == &node) {
                break;
            } else {
                return result;
            }
        }
    }

    return {StmtResultKind::Successful};
}

std::unique_ptr<Object> Evaluator::evaluateExpression(const BinaryOperation& node) {
    stackGuard();

    auto left = evaluateExpression(*node.left);
    auto right = evaluateExpression(*node.right);

    return left->binaryOperation(node.op, *right);
}
std::unique_ptr<Object> Evaluator::evaluateExpression(const CallFunction& node) {
    stackGuard();

    std::vector<std::unique_ptr<Object>> arguments;
    arguments.reserve(node.arguments.size());

    for (const auto& argument: node.arguments) {
        arguments.push_back(evaluateExpression(*argument));
    }

    return callFunction(*node.symbolRef, std::move(arguments));
}
std::unique_ptr<Object> Evaluator::evaluateExpression(const Number& node) {
    stackGuard();

    return std::make_unique<IntegerObject<int32_t>>(std::stoi(node.number));
}
std::unique_ptr<Object> Evaluator::evaluateExpression(const Identifier& node) {
    stackGuard();

    return _objects[node.symbolRef]->clone();
}
std::unique_ptr<Object> Evaluator::evaluateExpression(const PrefixOperation& node) {
    stackGuard();

    auto right = evaluateExpression(*node.right);

    return right->prefixOperation(node.op);
}
std::unique_ptr<Object> Evaluator::callFunction(const FunctionSymbol& functionSymbol, std::vector<std::unique_ptr<Object>> arguments) {
    stackGuard();

    size_t index = 0;
    for (const auto parameter: functionSymbol.getArguments()) {
        _objects[parameter] = std::move(arguments[index]);
        ++index;
    }
    auto result = evaluateStatement(*functionSymbol.getDefinition());

    return std::move(result.returnedObject);
}

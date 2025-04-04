// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <memory>

#include "Parser.hpp"

#include "AST/ASTNodes.hpp"
#include "Misc/Stack.hpp"

std::unique_ptr<Node> Parser::program() {
    stackGuard();

    auto program = declaration();
    if (_current != LexemType::EndOfFile) {
        throw std::logic_error("Syntax error");
    }

    return program;
}

std::unique_ptr<Declaration> Parser::declaration() {
    stackGuard();

    switch (_current.getType()) {
        case LexemType::Mod:
            return moduleDeclaration();
        case LexemType::Func:
            return functionDeclaration();
        default:
            throw std::logic_error("Syntax error");
    }
}
std::unique_ptr<DeclarationsBlock> Parser::declarationsBlock() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::LeftCurlyBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    std::vector<std::unique_ptr<Declaration>> declarations;
    while (_current != LexemType::RightCurlyBracket) {
        if (_current == LexemType::EndOfFile) {
            throw std::logic_error("Syntax error");
        }

        declarations.push_back(declaration());
    }

    if (_current != LexemType::RightCurlyBracket) {
        throw std::logic_error("Syntax error");
    }
    advance();

    return std::make_unique<DeclarationsBlock>(std::move(declarations),
                                               SourceLocation(startLocation,
                                                              _previous.getSourceLocation()));
}
std::unique_ptr<ModuleDeclaration> Parser::moduleDeclaration() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current.getType() != LexemType::Mod) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current.getType() != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }

    auto name = _current.getValue();
    advance();
    auto body = declarationsBlock();

    return std::make_unique<ModuleDeclaration>(std::move(name),
                                               std::move(body),
                                               SourceLocation(startLocation,
                                                              _previous.getSourceLocation()));
}
std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Func) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto name = _current.getValue();

    advance();
    if (_current != LexemType::LeftRoundBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    std::vector<std::unique_ptr<FunctionParameter>> parameters;
    while (_current != LexemType::RightRoundBracket) {
        auto parameter = functionParameter();
        parameters.push_back(std::move(parameter));

        if (_current == LexemType::Comma) {
            advance();
        }
    }

    advance();
    if (_current != LexemType::Arrow) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto returnType = _current.getValue();

    advance();
    auto functionBody = stmtBlock();

    return std::make_unique<FunctionDeclaration>(std::move(name),
                                                 std::move(returnType),
                                                 std::move(parameters),
                                                 std::move(functionBody),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
}
std::unique_ptr<FunctionParameter> Parser::functionParameter() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto type = _current.getValue();

    advance();
    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto name = _current.getValue();

    advance();
    return std::make_unique<FunctionParameter>(std::move(type),
                                               std::move(name),
                                               SourceLocation(startLocation,
                                                              _previous.getSourceLocation()));
}

std::unique_ptr<Statement> Parser::stmt() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    std::unique_ptr<Statement> node;
    switch (_current.getType()) {
        case LexemType::LeftCurlyBracket:
            node = stmtBlock();
            break;
        case LexemType::If:
            node = ifStmt();
            break;
        case LexemType::While:
            node = whileStmt();
            break;
        case LexemType::Break:
            node = breakStmt();
            break;
        case LexemType::Var:
            node = localVariableDeclaration();
            break;
        case LexemType::Return:
            node = returnStmt();
            break;
        case LexemType::Identifier:
            if (_current.getValue() == "print") {
                advance();
                auto printExpression = expr();

                node = std::make_unique<Print>(std::move(printExpression),
                                               SourceLocation(startLocation,
                                                              _previous.getSourceLocation()));
                break;
            }

            node = varAssignOrCallStmt();
            break;
        default:
            throw std::logic_error("Syntax error");
    }

    if (node->kind != Node::Kind::If &&
        node->kind != Node::Kind::While &&
        node->kind != Node::Kind::StatementBlock) {
        if (_current != LexemType::Semicolon) {
            throw std::logic_error("; expected");
        }
        advance();
    }

    return node;
}
std::unique_ptr<If> Parser::ifStmt() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::If) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current != LexemType::LeftRoundBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    auto condition = expr();
    if (_current != LexemType::RightRoundBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    auto then = stmtBlock();
    if (_current == LexemType::Else) {
        advance();

        std::unique_ptr<Statement> elseThen;
        if (_current == LexemType::If) {
            elseThen = ifStmt();
        } else {
            elseThen = stmtBlock();
        }

        return std::make_unique<If>(std::move(condition),
                                    std::move(then),
                                    std::move(elseThen),
                                    SourceLocation(startLocation,
                                                   _previous.getSourceLocation()));
    } else {
        return std::make_unique<If>(std::move(condition),
                                    std::move(then),
                                    nullptr,
                                    SourceLocation(startLocation,
                                                   _previous.getSourceLocation()));
    }
}
std::unique_ptr<While> Parser::whileStmt() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::While) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current != LexemType::LeftRoundBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    auto condition = expr();
    if (_current != LexemType::RightRoundBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current != LexemType::LeftCurlyBracket) {
        throw std::logic_error("Syntax error");
    }

    ++_loopNestingDepth;
    auto body = stmtBlock();
    --_loopNestingDepth;

    return std::make_unique<While>(std::move(condition),
                                   std::move(body),
                                   SourceLocation(startLocation,
                                                  _previous.getSourceLocation()));
}
std::unique_ptr<Break> Parser::breakStmt() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Break) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_loopNestingDepth > 0) {
        return std::make_unique<Break>(SourceLocation(startLocation,
                                                      _previous.getSourceLocation()));
    } else {
        throw std::logic_error("Break operator outisde loop body");
    }
}
std::unique_ptr<Return> Parser::returnStmt() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Return) {
        throw std::logic_error("Syntax error");
    }

    advance();
    auto returnExpression = expr();

    return std::make_unique<Return>(std::move(returnExpression),
                                    SourceLocation(startLocation,
                                                   _previous.getSourceLocation()));
}
std::unique_ptr<LocalVariableDeclaration> Parser::localVariableDeclaration() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Var) {
        throw std::logic_error("Syntax error");
    }

    advance();
    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto type = _current.getValue();

    advance();
    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto name = _current.getValue();

    advance();
    if (_current != LexemType::Assign) {
        throw std::logic_error("Syntax error");
    }

    advance();
    auto value = expr();

    return std::make_unique<LocalVariableDeclaration>(std::move(type),
                                                      std::move(name),
                                                      std::move(value),
                                                      SourceLocation(startLocation,
                                                                     _previous.getSourceLocation()));
}
std::unique_ptr<Statement> Parser::varAssignOrCallStmt() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Identifier) {
        throw std::logic_error("Syntax error");
    }
    auto name = _current.getValue();

    advance();
    if (_current != LexemType::Assign) {
        throw std::logic_error("Syntax error");
    }

    advance();
    auto value = expr();

    return std::make_unique<Assign>(name,
                                    std::move(value),
                                    SourceLocation(startLocation,
                                                   _previous.getSourceLocation()));
}
std::unique_ptr<StatementsBlock> Parser::stmtBlock() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::LeftCurlyBracket) {
        throw std::logic_error("Syntax error");
    }

    advance();
    std::vector<std::unique_ptr<Statement>> statements;
    while (_current != LexemType::RightCurlyBracket) {
        if (_current == LexemType::EndOfFile) {
            throw std::logic_error("Syntax error");
        }

        statements.push_back(stmt());
    }

    if (_current != LexemType::RightCurlyBracket) {
        throw std::logic_error("Syntax error");
    }
    advance();

    return std::make_unique<StatementsBlock>(std::move(statements),
                                             SourceLocation(startLocation,
                                                            _previous.getSourceLocation()));
}

std::unique_ptr<Expression> Parser::expr() {
    stackGuard();

    return logicOr();
}
std::unique_ptr<Expression> Parser::logicOr() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = logicAnd();

    while (_current == LexemType::LogicOr) {
        auto type = _current.getType();
        advance();
        auto right = logicAnd();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::logicAnd() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = bitOr();

    while (_current == LexemType::LogicAnd) {
        auto type = _current.getType();
        advance();
        auto right = bitOr();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitOr() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = bitXor();

    while (_current == LexemType::BitOr) {
        auto type = _current.getType();
        advance();
        auto right = bitXor();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitXor() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = bitAnd();

    while (_current == LexemType::BitXor) {
        auto type = _current.getType();
        advance();
        auto right = bitAnd();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitAnd() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = equal();

    while (_current == LexemType::BitAnd) {
        auto type = _current.getType();
        advance();
        auto right = equal();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::equal() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = relat();

    if (_current == LexemType::Equal ||
        _current == LexemType::Unequal) {
        auto type = _current.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::relat() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = term();

    if (_current == LexemType::Less ||
        _current == LexemType::LessEqual ||
        _current == LexemType::Greater ||
        _current == LexemType::GreaterEqual) {
        auto type = _current.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::term() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = fact();

    while (_current == LexemType::Plus ||
           _current == LexemType::Minus) {
        auto type = _current.getType();
        advance();
        auto right = fact();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::fact() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    auto node = unary();

    while (_current == LexemType::Multiply ||
           _current == LexemType::Divide ||
           _current == LexemType::Remainder) {
        auto type = _current.getType();
        advance();
        auto right = unary();
        node = std::make_unique<BinaryOperation>(type,
                                                 std::move(node),
                                                 std::move(right),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::unary() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    std::unique_ptr<Expression> node;

    if (_current == LexemType::Minus ||
        _current == LexemType::Not) {
        auto type = _current.getType();
        advance();
        node = std::make_unique<PrefixOperation>(type,
                                                 unary(),
                                                 SourceLocation(startLocation,
                                                                _previous.getSourceLocation()));
    } else {
        node = primary();
    }

    return node;
}
std::unique_ptr<Expression> Parser::primary() {
    stackGuard();
    auto startLocation = _current.getSourceLocation();

    if (_current == LexemType::Number) {
        auto value = _current.getValue();
        advance();
        return std::make_unique<Number>(std::move(value),
                                        SourceLocation(startLocation,
                                                       _previous.getSourceLocation()));
    } else if (_current == LexemType::Identifier) {
        auto name = _current.getValue();
        advance();
        if (_current != LexemType::LeftRoundBracket) {
            return std::make_unique<Identifier>(std::move(name),
                                                SourceLocation(startLocation,
                                                               _previous.getSourceLocation()));
        } else {
            advance();
            std::vector<std::unique_ptr<Expression>> arguments;
            while (_current != LexemType::RightRoundBracket) {
                auto expression = expr();
                arguments.push_back(std::move(expression));

                if (_current == LexemType::Comma) {
                    advance();
                }
            }
            advance();

            return std::make_unique<CallFunction>(std::move(name),
                                                  std::move(arguments),
                                                  SourceLocation(startLocation,
                                                                 _previous.getSourceLocation()));
        }
    } else if (_current == LexemType::LeftRoundBracket) {
        advance();
        auto expression = expr();

        if (_current.getType() == LexemType::RightRoundBracket) {
            advance();
            return expression;
        } else {
            throw std::logic_error("Right Bracket Expected");
        }
    }

    throw std::logic_error("Unexpected symbol");
}

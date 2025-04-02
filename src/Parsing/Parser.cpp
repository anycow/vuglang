// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <memory>

#include "Parser.hpp"

#include "AST/ASTNodes.hpp"
#include "Lexing/Lexer.hpp"
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
    auto node = std::make_unique<DeclarationsBlock>();
    if (_current == LexemType::LeftCurlyBracket) {
        advance();
        while (_current != LexemType::RightCurlyBracket) {
            if (_current == LexemType::EndOfFile) {
                throw std::logic_error("Syntax error");
            }

            node->declarations.push_back(declaration());
        }
        advance();
    } else {
        throw std::logic_error("Syntax error");
    }
    return node;
}
std::unique_ptr<ModuleDeclaration> Parser::moduleDeclaration() {
    stackGuard();

    if (_current.getType() == LexemType::Mod) {
        advance();
        if (_current.getType() == LexemType::Identifier) {
            auto name = _current.getValue();
            advance();
            auto body = declarationsBlock();
            return std::make_unique<ModuleDeclaration>(std::move(name), std::move(body));
        }
    }
    throw std::logic_error("Syntax error");
}
std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    stackGuard();

    if (_current == LexemType::Func) {
        advance();
        if (_current == LexemType::Identifier) {
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

            if (_current == LexemType::Arrow) {
                advance();
                if (_current == LexemType::Identifier) {
                    auto returnType = _current.getValue();
                    advance();
                    auto functionBody = stmtBlock();

                    return std::make_unique<FunctionDeclaration>(std::move(name),
                                                                 std::move(returnType),
                                                                 std::move(parameters),
                                                                 std::move(functionBody));
                }
            }
        }
    }

    throw std::logic_error("Syntax error");
}
std::unique_ptr<FunctionParameter> Parser::functionParameter() {
    if (_current == LexemType::Identifier) {
        auto type = _current.getValue();
        advance();

        if (_current == LexemType::Identifier) {
            auto name = _current.getValue();
            advance();

            return std::make_unique<FunctionParameter>(std::move(type),
                                                       std::move(name));
        }
    }

    throw std::logic_error("Syntax error");
}

std::unique_ptr<Statement> Parser::stmt() {
    stackGuard();

    switch (_current.getType()) {
        case LexemType::LeftCurlyBracket:
            return stmtBlock();
        case LexemType::If:
            return ifStmt();
        case LexemType::While:
            return whileStmt();
        case LexemType::Break:
            return breakStmt();
        case LexemType::Var:
            return localVarDeclaration();
        case LexemType::Return:
            return returnStmt();
        case LexemType::Identifier:
            if (_current.getValue() == "print") {
                advance();
                auto printExpression = expr();
                advance();

                return std::make_unique<Print>(std::move(printExpression));
            }

            return varAssignOrCallStmt();
        default:
            throw std::logic_error("Syntax error");
    }
}
std::unique_ptr<If> Parser::ifStmt() {
    stackGuard();

    if (_current == LexemType::If) {
        advance();

        if (_current == LexemType::LeftRoundBracket) {
            advance();
            auto condition = expr();

            if (_current == LexemType::RightRoundBracket) {
                advance();
                auto then = stmtBlock();
                auto node = std::make_unique<If>(std::move(condition), std::move(then));

                if (_current == LexemType::Else) {
                    advance();

                    if (_current == LexemType::If) {
                        node->elseThen = ifStmt();

                        return node;
                    } else {
                        node->elseThen = stmtBlock();

                        return node;
                    }
                } else {
                    return node;
                }
            }
        }
    }

    throw std::logic_error("Unexpected symbol");
}
std::unique_ptr<While> Parser::whileStmt() {
    stackGuard();

    if (_current == LexemType::While) {
        advance();

        if (_current == LexemType::LeftRoundBracket) {
            advance();
            auto condition = expr();

            if (_current == LexemType::RightRoundBracket) {
                advance();

                if (_current == LexemType::LeftCurlyBracket) {
                    ++_loopNestingDepth;
                    auto body = stmtBlock();
                    ++_loopNestingDepth;

                    return std::make_unique<While>(std::move(condition), std::move(body));
                }
            }
        }
    }
    throw std::logic_error("Syntax error");
}
std::unique_ptr<Break> Parser::breakStmt() {
    stackGuard();

    if (_loopNestingDepth > 0) {
        advance();
        advance();
        return std::make_unique<Break>();
    } else {
        throw std::logic_error("Break operator outisde loop body");
    }
}
std::unique_ptr<Return> Parser::returnStmt() {
    stackGuard();

    advance();
    auto returnExpression = expr();
    advance();

    return std::make_unique<Return>(std::move(returnExpression));
}
std::unique_ptr<LocalVariableDeclaration> Parser::localVarDeclaration() {
    stackGuard();

    advance();
    if (_current == LexemType::Identifier) {
        auto type = _current.getValue();
        advance();

        if (_current == LexemType::Identifier) {
            auto name = _current.getValue();
            auto declaration = std::make_unique<LocalVariableDeclaration>(std::move(type),
                                                                          std::move(name));
            advance();

            if (_current == LexemType::Assign) {
                advance();
                declaration->value = expr();
            }
            if (_current == LexemType::Semicolon) {
                advance();
                return declaration;
            }

            throw std::logic_error("; expected");
        } else {
            throw std::logic_error("Syntax error");
        }
    } else {
        throw std::logic_error("Syntax error");
    }
}
std::unique_ptr<Statement> Parser::varAssignOrCallStmt() {
    stackGuard();

    if (_current == LexemType::Identifier) {
        auto id = _current.getValue();
        advance();

        if (_current == LexemType::Assign) {
            advance();
            auto expression = expr();

            if (_current == LexemType::Semicolon) {
                advance();
                return std::make_unique<Assign>(std::move(id),
                                                std::move(expression));
            } else {
                throw std::logic_error("; expected");
            }
        }
        //        else if (_current == LexemType::LeftRoundBracket) {
        //            advance();
        //
        //            std::vector<std::unique_ptr<Expression>> arguments;
        //            while (_current != LexemType::RightRoundBracket) {
        //                auto expression = expr();
        //                arguments.push_back(std::move(expression));
        //
        //                if (_current == LexemType::Comma) {
        //                    advance();
        //                }
        //            }
        //
        //            advance();
        //            if (_current != LexemType::Semicolon) {
        //                throw std::logic_error("Expected ;");
        //            }
        //            advance();
        //
        //            return std::make_unique<CallFunction>(std::move(id), std::move(arguments));
        //        }
    }

    throw std::logic_error("Syntax error");
}
std::unique_ptr<StatementsBlock> Parser::stmtBlock() {
    stackGuard();

    auto node = std::make_unique<StatementsBlock>();
    if (_current == LexemType::LeftCurlyBracket) {
        advance();
        while (_current != LexemType::RightCurlyBracket && _current != LexemType::EndOfFile) {
            node->statements.push_back(stmt());
        }
        advance();
    } else {
        throw std::logic_error("Syntax error");
    }
    return node;
}

std::unique_ptr<Expression> Parser::expr() {
    stackGuard();

    return logicOr();
}
std::unique_ptr<Expression> Parser::logicOr() {
    stackGuard();

    auto node = logicAnd();

    while (_current == LexemType::LogicOr) {
        auto type = _current.getType();
        advance();
        auto right = logicAnd();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::logicAnd() {
    stackGuard();

    auto node = bitOr();

    while (_current == LexemType::LogicAnd) {
        auto type = _current.getType();
        advance();
        auto right = bitOr();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitOr() {
    stackGuard();

    auto node = bitXor();

    while (_current == LexemType::BitOr) {
        auto type = _current.getType();
        advance();
        auto right = bitXor();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitXor() {
    stackGuard();

    auto node = bitAnd();

    while (_current == LexemType::BitXor) {
        auto type = _current.getType();
        advance();
        auto right = bitAnd();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitAnd() {
    stackGuard();

    auto node = equal();

    while (_current == LexemType::BitAnd) {
        auto type = _current.getType();
        advance();
        auto right = equal();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::equal() {
    stackGuard();

    auto node = relat();

    if (_current == LexemType::Equal ||
        _current == LexemType::Unequal) {
        auto type = _current.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::relat() {
    stackGuard();

    auto node = term();

    if (_current == LexemType::Less ||
        _current == LexemType::LessEqual ||
        _current == LexemType::Greater ||
        _current == LexemType::GreaterEqual) {
        auto type = _current.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::term() {
    stackGuard();

    auto node = fact();

    while (_current == LexemType::Plus ||
           _current == LexemType::Minus) {
        auto type = _current.getType();
        advance();
        auto right = fact();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::fact() {
    stackGuard();

    auto node = unary();

    while (_current == LexemType::Multiply ||
           _current == LexemType::Divide ||
           _current == LexemType::Remainder) {
        auto type = _current.getType();
        advance();
        auto right = unary();
        node = std::make_unique<BinaryOperation>(type, std::move(node), std::move(right));
    }

    return node;
}
std::unique_ptr<Expression> Parser::unary() {
    stackGuard();

    std::unique_ptr<Expression> node;

    if (_current == LexemType::Minus ||
        _current == LexemType::Not) {
        auto type = _current.getType();
        advance();
        node = std::make_unique<PrefixOperation>(type, unary());
    } else {
        node = primary();
    }

    return node;
}
std::unique_ptr<Expression> Parser::primary() {
    stackGuard();

    if (_current == LexemType::Number) {
        auto value = _current.getValue();
        advance();
        return std::make_unique<Number>(value);
    } else if (_current == LexemType::Identifier) {
        auto name = _current.getValue();
        advance();
        if (_current != LexemType::LeftRoundBracket) {
            return std::make_unique<Identifier>(std::move(name));
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

            return std::make_unique<CallFunction>(std::move(name), std::move(arguments));
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

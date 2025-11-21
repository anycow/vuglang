// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.


#include <memory>

#include "Parser.hpp"

#include "AST/ASTNodes.hpp"
#include "Diagnostic/DiagnosticManager.hpp"
#include "Misc/Stack.hpp"

std::unique_ptr<Node> Parser::program() {
    stackGuard();

    auto program = declaration();
    if (_current != LexemType::EndOfFile) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "unexpected token",
                                                {_previous.getSourceLocation()}));
        _diagnosticManager.report(diagnostic);
    }

    return program;
}

std::unique_ptr<Declaration> Parser::declaration() {
    stackGuard();

    try {
        switch (_current.getType()) {
            case LexemType::Mod:
                return moduleDeclaration();
            case LexemType::Func:
                return functionDeclaration();
            default:
                throw std::logic_error("Syntax error");
        }
    } catch (ParsingException& exception) {
        _diagnosticManager.report(exception.getDiagnostic());

        int64_t level = 0;
        while (true) {
            if (_current == LexemType::LeftCurlyBracket) {
                level++;
            } else if (_current == LexemType::RightCurlyBracket) {
                level--;
                if (level == 0) {
                    advance();
                    break;
                }
            }

            if (level < 0) {
                break;
            }
            if (level == 0 && _current == LexemType::Semicolon) {
                advance();
                break;
            }
            advance();
        }
        return std::make_unique<BadDeclaration>();
    }
}
std::unique_ptr<DeclarationsBlock> Parser::declarationsBlock() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::LeftCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '{'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    std::vector<std::unique_ptr<Declaration>> declarations;
    while (_current != LexemType::RightCurlyBracket) {
        if (_current == LexemType::EndOfFile) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected '}'",
                                                    {_previous.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }

        declarations.push_back(declaration());
    }

    if (_current != LexemType::RightCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '}'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    advance();

    return std::make_unique<DeclarationsBlock>(
        std::move(declarations),
        SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<ModuleDeclaration> Parser::moduleDeclaration() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current.getType() != LexemType::Mod) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'mod'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_current.getType() != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected module name",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    auto name = _current.getValue();
    advance();
    auto body = declarationsBlock();

    return std::make_unique<ModuleDeclaration>(
        std::move(name),
        std::move(body),
        SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Func) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'func'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected function name",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = _current.getValue();

    advance();
    if (_current != LexemType::LeftRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '('",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    std::vector<std::unique_ptr<FunctionParameter>> parameters;
    while (_current != LexemType::RightRoundBracket) {
        auto parameter = functionParameter();
        parameters.push_back(std::move(parameter));

        if (_current == LexemType::Comma) {
            advance();
        } else if (_current != LexemType::RightRoundBracket) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected ',' or ')'",
                                                    {_previous.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }
    }

    advance();
    if (_current != LexemType::Arrow) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '->' before function type",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected function type name",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto returnType = _current.getValue();

    advance();
    auto functionBody = stmtBlock();

    return std::make_unique<FunctionDeclaration>(
        std::move(name),
        std::move(returnType),
        std::move(parameters),
        std::move(functionBody),
        SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<FunctionParameter> Parser::functionParameter() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected parameter type name",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto type = _current.getValue();

    advance();
    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected parameter name",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = _current.getValue();

    advance();
    return std::make_unique<FunctionParameter>(
        std::move(type),
        std::move(name),
        SourceLocation(startLocation, _previous.getSourceLocation()));
}

std::unique_ptr<Statement> Parser::stmt() {
    try {
        stackGuard();
        const auto startLocation = _current.getSourceLocation();

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

                    node = std::make_unique<Print>(
                        std::move(printExpression),
                        SourceLocation(startLocation, _previous.getSourceLocation()));
                    break;
                }

                node = varAssign();
                break;
            default: {
                auto diagnostic = Diagnostic();
                diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                        "unexpected token",
                                                        {_previous.getSourceLocation()}));
                throw ParsingException(std::move(diagnostic));
            }
        }

        if (node->kind != Node::Kind::If && node->kind != Node::Kind::While
            && node->kind != Node::Kind::StatementBlock) {
            if (_current != LexemType::Semicolon) {
                auto diagnostic = Diagnostic();

                const auto errorLine = _previous.getSourceLocation().getSourceFile()->getLine(
                    _previous.getSourceLocation().getEndLine());
                auto fixLine = std::string(errorLine.begin(), errorLine.end());
                fixLine.insert(fixLine.begin() + _previous.getSourceLocation().getEndColumn(), ';');

                diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                        "expected semicolon",
                                                        {_previous.getSourceLocation()})
                                          .addFix(DiagnosticFix().addDiff(
                                              *_previous.getSourceLocation().getSourceFile(),
                                              _previous.getSourceLocation().getEndLine(),
                                              fixLine)));
                throw ParsingException(std::move(diagnostic));
            }
            advance();
        }

        return node;
    } catch (ParsingException& exception) {
        _diagnosticManager.report(exception.getDiagnostic());

        int64_t level = 0;
        while (true) {
            if (_current == LexemType::LeftCurlyBracket) {
                level++;
            } else if (_current == LexemType::RightCurlyBracket) {
                level--;
            }

            if (level < 0) {
                break;
            }
            if (level == 0 && _current == LexemType::Semicolon) {
                advance();
                break;
            }
            advance();
        }
        return std::make_unique<BadStatement>();
    }
}
std::unique_ptr<If> Parser::ifStmt() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::If) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'if'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_current != LexemType::LeftRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '(' after 'if'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto condition = expr();
    if (_current != LexemType::RightRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected ')'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
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
                                    SourceLocation(startLocation, _previous.getSourceLocation()));
    } else {
        return std::make_unique<If>(std::move(condition),
                                    std::move(then),
                                    nullptr,
                                    SourceLocation(startLocation, _previous.getSourceLocation()));
    }
}
std::unique_ptr<While> Parser::whileStmt() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::While) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'while'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_current != LexemType::LeftRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '(' after 'while'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto condition = expr();
    if (_current != LexemType::RightRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected ')'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();

    ++_loopNestingDepth;
    auto body = stmtBlock();
    --_loopNestingDepth;

    return std::make_unique<While>(std::move(condition),
                                   std::move(body),
                                   SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<Break> Parser::breakStmt() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Break) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'break'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_loopNestingDepth > 0) {
        return std::make_unique<Break>(
            SourceLocation(startLocation, _previous.getSourceLocation()));
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "break operator outside loop body",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
}
std::unique_ptr<Return> Parser::returnStmt() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Return) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'return'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto returnExpression = expr();

    return std::make_unique<Return>(std::move(returnExpression),
                                    SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<LocalVariableDeclaration> Parser::localVariableDeclaration() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Var) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'var'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected type name in var declaration",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto type = _current.getValue();

    advance();
    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected variable name in var declaration",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = _current.getValue();

    advance();
    if (_current != LexemType::Assign) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '=' after var declaration",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto value = expr();

    return std::make_unique<LocalVariableDeclaration>(
        std::move(type),
        std::move(name),
        std::move(value),
        SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<Statement> Parser::varAssign() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected variable name",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = _current.getValue();

    advance();
    if (_current != LexemType::Assign) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '='",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto value = expr();

    return std::make_unique<Assign>(name,
                                    std::move(value),
                                    SourceLocation(startLocation, _previous.getSourceLocation()));
}
std::unique_ptr<StatementsBlock> Parser::stmtBlock() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current != LexemType::LeftCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '{'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    std::vector<std::unique_ptr<Statement>> statements;
    while (_current != LexemType::RightCurlyBracket) {
        if (_current == LexemType::EndOfFile) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected '}'",
                                                    {_previous.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }

        statements.push_back(stmt());
    }

    if (_current != LexemType::RightCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '}'",
                                                {_previous.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    advance();

    return std::make_unique<StatementsBlock>(
        std::move(statements),
        SourceLocation(startLocation, _previous.getSourceLocation()));
}

std::unique_ptr<Expression> Parser::expr() {
    stackGuard();

    return logicOr();
}
std::unique_ptr<Expression> Parser::logicOr() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = logicAnd();

    while (_current == LexemType::LogicOr) {
        auto type = _current.getType();
        advance();
        auto right = logicAnd();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::logicAnd() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = bitOr();

    while (_current == LexemType::LogicAnd) {
        auto type = _current.getType();
        advance();
        auto right = bitOr();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitOr() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = bitXor();

    while (_current == LexemType::BitOr) {
        auto type = _current.getType();
        advance();
        auto right = bitXor();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitXor() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = bitAnd();

    while (_current == LexemType::BitXor) {
        auto type = _current.getType();
        advance();
        auto right = bitAnd();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitAnd() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = equal();

    while (_current == LexemType::BitAnd) {
        auto type = _current.getType();
        advance();
        auto right = equal();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::equal() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = relat();

    if (_current == LexemType::Equal || _current == LexemType::Unequal) {
        auto type = _current.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::relat() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = term();

    if (_current == LexemType::Less || _current == LexemType::LessEqual
        || _current == LexemType::Greater || _current == LexemType::GreaterEqual) {
        auto type = _current.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::term() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = fact();

    while (_current == LexemType::Plus || _current == LexemType::Minus) {
        auto type = _current.getType();
        advance();
        auto right = fact();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::fact() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    auto node = unary();

    while (_current == LexemType::Multiply || _current == LexemType::Divide
           || _current == LexemType::Remainder) {
        auto type = _current.getType();
        advance();
        auto right = unary();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::unary() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    std::unique_ptr<Expression> node;

    if (_current == LexemType::Minus || _current == LexemType::Not) {
        auto type = _current.getType();
        advance();
        node = std::make_unique<PrefixOperation>(
            type,
            unary(),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    } else {
        node = primary();
    }

    return node;
}
std::unique_ptr<Expression> Parser::primary() {
    stackGuard();
    const auto startLocation = _current.getSourceLocation();

    if (_current == LexemType::Number) {
        auto value = _current.getValue();
        advance();
        return std::make_unique<Number>(
            std::move(value),
            SourceLocation(startLocation, _previous.getSourceLocation()));
    } else if (_current == LexemType::Identifier) {
        auto name = _current.getValue();
        advance();
        if (_current != LexemType::LeftRoundBracket) {
            return std::make_unique<Identifier>(
                std::move(name),
                SourceLocation(startLocation, _previous.getSourceLocation()));
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

            return std::make_unique<CallFunction>(
                std::move(name),
                std::move(arguments),
                SourceLocation(startLocation, _previous.getSourceLocation()));
        }
    } else if (_current == LexemType::LeftRoundBracket) {
        advance();
        auto expression = expr();

        if (_current.getType() == LexemType::RightRoundBracket) {
            advance();
            return expression;
        } else {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected '}",
                                                    {_previous.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }
    }

    auto diagnostic = Diagnostic();
    diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                            "unexpected token",
                                            {_previous.getSourceLocation()}));
    throw ParsingException(std::move(diagnostic));
}

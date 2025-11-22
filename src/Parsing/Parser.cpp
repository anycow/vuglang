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
    if (mCurrent != LexemType::EndOfFile) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "unexpected token",
                                                {mPrevious.getSourceLocation()}));
        mDiagnosticManager.report(diagnostic);
    }

    return program;
}

std::unique_ptr<Declaration> Parser::declaration() {
    stackGuard();

    try {
        switch (mCurrent.getType()) {
            case LexemType::Mod:
                return moduleDeclaration();
            case LexemType::Func:
                return functionDeclaration();
            default:
                throw std::logic_error("Syntax error");
        }
    } catch (ParsingException& exception) {
        mDiagnosticManager.report(exception.getDiagnostic());

        int64_t level = 0;
        while (true) {
            if (mCurrent == LexemType::LeftCurlyBracket) {
                level++;
            } else if (mCurrent == LexemType::RightCurlyBracket) {
                level--;
                if (level == 0) {
                    advance();
                    break;
                }
            }

            if (level < 0) {
                break;
            }
            if (level == 0 && mCurrent == LexemType::Semicolon) {
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
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::LeftCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '{'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    std::vector<std::unique_ptr<Declaration>> declarations;
    while (mCurrent != LexemType::RightCurlyBracket) {
        if (mCurrent == LexemType::EndOfFile) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected '}'",
                                                    {mPrevious.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }

        declarations.push_back(declaration());
    }

    if (mCurrent != LexemType::RightCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '}'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    advance();

    return std::make_unique<DeclarationsBlock>(
        std::move(declarations),
        SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<ModuleDeclaration> Parser::moduleDeclaration() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent.getType() != LexemType::Mod) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'mod'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mCurrent.getType() != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected module name",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    auto name = mCurrent.getValue();
    advance();
    auto body = declarationsBlock();

    return std::make_unique<ModuleDeclaration>(
        std::move(name),
        std::move(body),
        SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<FunctionDeclaration> Parser::functionDeclaration() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::Func) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'func'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected function name",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = mCurrent.getValue();

    advance();
    if (mCurrent != LexemType::LeftRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '('",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    std::vector<std::unique_ptr<FunctionParameter>> parameters;
    while (mCurrent != LexemType::RightRoundBracket) {
        auto parameter = functionParameter();
        parameters.push_back(std::move(parameter));

        if (mCurrent == LexemType::Comma) {
            advance();
        } else if (mCurrent != LexemType::RightRoundBracket) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected ',' or ')'",
                                                    {mPrevious.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }
    }

    advance();
    if (mCurrent != LexemType::Arrow) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '->' before function type",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected function type name",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto returnType = mCurrent.getValue();

    advance();
    auto functionBody = stmtBlock();

    return std::make_unique<FunctionDeclaration>(
        std::move(name),
        std::move(returnType),
        std::move(parameters),
        std::move(functionBody),
        SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<FunctionParameter> Parser::functionParameter() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected parameter type name",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto type = mCurrent.getValue();

    advance();
    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected parameter name",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = mCurrent.getValue();

    advance();
    return std::make_unique<FunctionParameter>(
        std::move(type),
        std::move(name),
        SourceLocation(startLocation, mPrevious.getSourceLocation()));
}

std::unique_ptr<Statement> Parser::stmt() {
    try {
        stackGuard();
        const auto startLocation = mCurrent.getSourceLocation();

        std::unique_ptr<Statement> node;
        switch (mCurrent.getType()) {
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
                if (mCurrent.getValue() == "print") {
                    advance();
                    auto printExpression = expr();

                    node = std::make_unique<Print>(
                        std::move(printExpression),
                        SourceLocation(startLocation, mPrevious.getSourceLocation()));
                    break;
                }

                node = varAssign();
                break;
            default: {
                auto diagnostic = Diagnostic();
                diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                        "unexpected token",
                                                        {mPrevious.getSourceLocation()}));
                throw ParsingException(std::move(diagnostic));
            }
        }

        if (node->kind != Node::Kind::If && node->kind != Node::Kind::While
            && node->kind != Node::Kind::StatementBlock) {
            if (mCurrent != LexemType::Semicolon) {
                auto diagnostic = Diagnostic();

                const auto errorLine = mPrevious.getSourceLocation().getSourceFile()->getLine(
                    mPrevious.getSourceLocation().getEndLine());
                auto fixLine = std::string(errorLine.begin(), errorLine.end());
                fixLine.insert(fixLine.begin() + mPrevious.getSourceLocation().getEndColumn(), ';');

                diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                        "expected semicolon",
                                                        {mPrevious.getSourceLocation()})
                                          .addFix(DiagnosticFix().addDiff(
                                              *mPrevious.getSourceLocation().getSourceFile(),
                                              mPrevious.getSourceLocation().getEndLine(),
                                              fixLine)));
                throw ParsingException(std::move(diagnostic));
            }
            advance();
        }

        return node;
    } catch (ParsingException& exception) {
        mDiagnosticManager.report(exception.getDiagnostic());

        int64_t level = 0;
        while (true) {
            if (mCurrent == LexemType::LeftCurlyBracket) {
                level++;
            } else if (mCurrent == LexemType::RightCurlyBracket) {
                level--;
            }

            if (level < 0) {
                break;
            }
            if (level == 0 && mCurrent == LexemType::Semicolon) {
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
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::If) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'if'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mCurrent != LexemType::LeftRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '(' after 'if'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto condition = expr();
    if (mCurrent != LexemType::RightRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected ')'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto then = stmtBlock();
    if (mCurrent == LexemType::Else) {
        advance();

        std::unique_ptr<Statement> elseThen;
        if (mCurrent == LexemType::If) {
            elseThen = ifStmt();
        } else {
            elseThen = stmtBlock();
        }

        return std::make_unique<If>(std::move(condition),
                                    std::move(then),
                                    std::move(elseThen),
                                    SourceLocation(startLocation, mPrevious.getSourceLocation()));
    } else {
        return std::make_unique<If>(std::move(condition),
                                    std::move(then),
                                    nullptr,
                                    SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }
}
std::unique_ptr<While> Parser::whileStmt() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::While) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'while'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mCurrent != LexemType::LeftRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '(' after 'while'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto condition = expr();
    if (mCurrent != LexemType::RightRoundBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected ')'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();

    ++mLoopNestingDepth;
    auto body = stmtBlock();
    --mLoopNestingDepth;

    return std::make_unique<While>(std::move(condition),
                                   std::move(body),
                                   SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<Break> Parser::breakStmt() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::Break) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'break'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mLoopNestingDepth > 0) {
        return std::make_unique<Break>(
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    } else {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "break operator outside loop body",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
}
std::unique_ptr<Return> Parser::returnStmt() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::Return) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'return'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto returnExpression = expr();

    return std::make_unique<Return>(std::move(returnExpression),
                                    SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<LocalVariableDeclaration> Parser::localVariableDeclaration() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::Var) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected 'var'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected type name in var declaration",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto type = mCurrent.getValue();

    advance();
    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected variable name in var declaration",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = mCurrent.getValue();

    advance();
    if (mCurrent != LexemType::Assign) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '=' after var declaration",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto value = expr();

    return std::make_unique<LocalVariableDeclaration>(
        std::move(type),
        std::move(name),
        std::move(value),
        SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<Statement> Parser::varAssign() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::Identifier) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected variable name",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    auto name = mCurrent.getValue();

    advance();
    if (mCurrent != LexemType::Assign) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '='",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    auto value = expr();

    return std::make_unique<Assign>(name,
                                    std::move(value),
                                    SourceLocation(startLocation, mPrevious.getSourceLocation()));
}
std::unique_ptr<StatementsBlock> Parser::stmtBlock() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent != LexemType::LeftCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '{'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }

    advance();
    std::vector<std::unique_ptr<Statement>> statements;
    while (mCurrent != LexemType::RightCurlyBracket) {
        if (mCurrent == LexemType::EndOfFile) {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected '}'",
                                                    {mPrevious.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }

        statements.push_back(stmt());
    }

    if (mCurrent != LexemType::RightCurlyBracket) {
        auto diagnostic = Diagnostic();
        diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                "expected '}'",
                                                {mPrevious.getSourceLocation()}));
        throw ParsingException(std::move(diagnostic));
    }
    advance();

    return std::make_unique<StatementsBlock>(
        std::move(statements),
        SourceLocation(startLocation, mPrevious.getSourceLocation()));
}

std::unique_ptr<Expression> Parser::expr() {
    stackGuard();

    return logicOr();
}
std::unique_ptr<Expression> Parser::logicOr() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = logicAnd();

    while (mCurrent == LexemType::LogicOr) {
        auto type = mCurrent.getType();
        advance();
        auto right = logicAnd();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::logicAnd() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = bitOr();

    while (mCurrent == LexemType::LogicAnd) {
        auto type = mCurrent.getType();
        advance();
        auto right = bitOr();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitOr() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = bitXor();

    while (mCurrent == LexemType::BitOr) {
        auto type = mCurrent.getType();
        advance();
        auto right = bitXor();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitXor() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = bitAnd();

    while (mCurrent == LexemType::BitXor) {
        auto type = mCurrent.getType();
        advance();
        auto right = bitAnd();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::bitAnd() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = equal();

    while (mCurrent == LexemType::BitAnd) {
        auto type = mCurrent.getType();
        advance();
        auto right = equal();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::equal() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = relat();

    if (mCurrent == LexemType::Equal || mCurrent == LexemType::Unequal) {
        auto type = mCurrent.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::relat() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = term();

    if (mCurrent == LexemType::Less || mCurrent == LexemType::LessEqual
        || mCurrent == LexemType::Greater || mCurrent == LexemType::GreaterEqual) {
        auto type = mCurrent.getType();
        advance();
        auto right = relat();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::term() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = fact();

    while (mCurrent == LexemType::Plus || mCurrent == LexemType::Minus) {
        auto type = mCurrent.getType();
        advance();
        auto right = fact();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::fact() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    auto node = unary();

    while (mCurrent == LexemType::Multiply || mCurrent == LexemType::Divide
           || mCurrent == LexemType::Remainder) {
        auto type = mCurrent.getType();
        advance();
        auto right = unary();
        node = std::make_unique<BinaryOperation>(
            type,
            std::move(node),
            std::move(right),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    }

    return node;
}
std::unique_ptr<Expression> Parser::unary() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    std::unique_ptr<Expression> node;

    if (mCurrent == LexemType::Minus || mCurrent == LexemType::Not) {
        auto type = mCurrent.getType();
        advance();
        node = std::make_unique<PrefixOperation>(
            type,
            unary(),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    } else {
        node = primary();
    }

    return node;
}
std::unique_ptr<Expression> Parser::primary() {
    stackGuard();
    const auto startLocation = mCurrent.getSourceLocation();

    if (mCurrent == LexemType::Number) {
        auto value = mCurrent.getValue();
        advance();
        return std::make_unique<Number>(
            std::move(value),
            SourceLocation(startLocation, mPrevious.getSourceLocation()));
    } else if (mCurrent == LexemType::Identifier) {
        auto name = mCurrent.getValue();
        advance();
        if (mCurrent != LexemType::LeftRoundBracket) {
            return std::make_unique<Identifier>(
                std::move(name),
                SourceLocation(startLocation, mPrevious.getSourceLocation()));
        } else {
            advance();
            std::vector<std::unique_ptr<Expression>> arguments;
            while (mCurrent != LexemType::RightRoundBracket) {
                auto expression = expr();
                arguments.push_back(std::move(expression));

                if (mCurrent == LexemType::Comma) {
                    advance();
                }
            }
            advance();

            return std::make_unique<CallFunction>(
                std::move(name),
                std::move(arguments),
                SourceLocation(startLocation, mPrevious.getSourceLocation()));
        }
    } else if (mCurrent == LexemType::LeftRoundBracket) {
        advance();
        auto expression = expr();

        if (mCurrent.getType() == LexemType::RightRoundBracket) {
            advance();
            return expression;
        } else {
            auto diagnostic = Diagnostic();
            diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                                    "expected '}",
                                                    {mPrevious.getSourceLocation()}));
            throw ParsingException(std::move(diagnostic));
        }
    }

    auto diagnostic = Diagnostic();
    diagnostic.addMessage(DiagnosticMessage(DiagnosticMessage::Severity::Error,
                                            "unexpected token",
                                            {mPrevious.getSourceLocation()}));
    throw ParsingException(std::move(diagnostic));
}

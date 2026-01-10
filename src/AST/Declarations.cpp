// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "AST/Declarations.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "AST/Node.hpp"
#include "AST/Statements.hpp"
#include "Lexing/Token.hpp"

// Constructors and destructors moved to .cpp to avoid cyclic dependencies with unique_ptr
Declaration::Declaration(const Kind nodeType, const SourceLocation& sourceLocation)
    : Node(nodeType, sourceLocation) {
}

BadDeclaration::BadDeclaration()
    : Declaration(Kind::BadDeclaration, SourceLocation()) {
}
BadDeclaration::~BadDeclaration() = default;

DeclarationsBlock::DeclarationsBlock(Token leftBracket,
                                     Token rightBracket,
                                     std::vector<std::unique_ptr<Declaration>> declarations,
                                     const SourceLocation& sourceLocation)
    : Declaration(Kind::DeclarationsBlock, sourceLocation),
      mLeftBracket(std::move(leftBracket)),
      mRightBracket(std::move(rightBracket)),
      mDeclarations(std::move(declarations)) {
}
DeclarationsBlock::~DeclarationsBlock() = default;

FunctionParameter::FunctionParameter(Token type, Token name, const SourceLocation& sourceLocation)
    : Declaration(Kind::FunctionParameter, sourceLocation),
      mType(std::move(type)),
      mName(std::move(name)) {
}
FunctionParameter::~FunctionParameter() = default;

FunctionDeclaration::FunctionDeclaration(Token funcKeyword,
                                         Token name,
                                         Token leftBracket,
                                         Token rightBracket,
                                         Token arrow,
                                         Token returnType,
                                         std::vector<std::unique_ptr<FunctionParameter>> parameters,
                                         std::unique_ptr<StatementsBlock> definition,
                                         const SourceLocation& sourceLocation)
    : Declaration(Kind::FunctionDeclaration, sourceLocation),
      mFuncKeyword(std::move(funcKeyword)),
      mName(std::move(name)),
      mLeftBracket(std::move(leftBracket)),
      mRightBracket(std::move(rightBracket)),
      mArrow(std::move(arrow)),
      mReturnType(std::move(returnType)),
      mParameters(std::move(parameters)),
      mDefinition(std::move(definition)) {
}
FunctionDeclaration::~FunctionDeclaration() = default;

ModuleDeclaration::ModuleDeclaration(Token modKeyword,
                                     Token name,
                                     std::unique_ptr<DeclarationsBlock> body,
                                     const SourceLocation& sourceLocation)
    : Declaration(Kind::ModuleDeclaration, sourceLocation),
      mModKeyword(std::move(modKeyword)),
      mName(std::move(name)),
      mBody(std::move(body)) {
}
ModuleDeclaration::~ModuleDeclaration() = default;

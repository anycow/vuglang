// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_DECLARATION_HPP
#define VUG_DECLARATION_HPP

#include "AST/Nodes/Node.hpp"

class Symbol;

struct Declaration : public Node {
    Declaration(const Kind nodeType, const SourceLocation& sourceLocation)
        : Node(nodeType, sourceLocation) {
    }

    bool isDeclaration() const override {
        return true;
    }

    [[nodiscard]] virtual Symbol* getSymbolPtr() const {
        return nullptr;
    }
};

struct BadDeclaration : public Declaration {
    BadDeclaration()
        : Declaration(Kind::BadDeclaration, SourceLocation()) {
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    [[nodiscard]] bool isInvalid() const override {
        return true;
    }
};

#endif  // VUG_DECLARATION_HPP

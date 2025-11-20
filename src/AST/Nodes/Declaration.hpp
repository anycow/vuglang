// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_DECLARATION_HPP
#define VUG_DECLARATION_HPP

#include "AST/Nodes/Node.hpp"

#include "Evaluator/Evaluator.hpp"

class Symbol;

struct Declaration : public Node {
    explicit Declaration(Kind nodeType, SourceLocation sourceLocation)
        : Node(nodeType, sourceLocation) {}

    bool isDeclaration() override {
        return true;
    }

    [[nodiscard]] virtual Symbol* getSymbolPtr() const { return nullptr; }

    virtual void evaluate(Evaluator& evaluator) { throw std::logic_error("Not implemented"); }
};

struct BadDeclaration : public Declaration {
    explicit BadDeclaration()
        : Declaration(Kind::BadDeclaration, SourceLocation()) {}

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
    bool isInvalid() override {
        return true;
    }

    void evaluate(Evaluator& evaluator) override { throw std::logic_error("Not implemented"); }
};

#endif//VUG_DECLARATION_HPP

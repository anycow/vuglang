// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_MODULEDECLARATION_HPP
#define VUG_MODULEDECLARATION_HPP

#include "AST/Nodes/Declaration.hpp"

#include "AST/Nodes/Declarations/DeclarationsBlock.hpp"
#include "AST/Nodes/Expressions/Identifier.hpp"

struct ModuleDeclaration : public Declaration {
    std::string name;
    std::unique_ptr<DeclarationsBlock> body;

    ModuleSymbol* symbolRef{nullptr};

    ModuleDeclaration(std::string name,
                      std::unique_ptr<DeclarationsBlock> body,
                      const SourceLocation& sourceLocation)
        : Declaration(Kind::ModuleDeclaration, sourceLocation),
          name(std::move(name)),
          body(std::move(body)) {
    }

    [[nodiscard]] Symbol* getSymbolPtr() const override {
        return symbolRef;
    }

    void evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateDeclaration(*this);
    }
    void accept(ASTWalker& walker) override {
        return walker.visit(*this);
    }
};

#endif  // VUG_MODULEDECLARATION_HPP

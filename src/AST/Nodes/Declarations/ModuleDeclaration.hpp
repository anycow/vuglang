// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_MODULEDECLARATION_HPP
#define VUG_MODULEDECLARATION_HPP

#include <memory>
#include <string>
#include <utility>

#include "AST/ASTWalker.hpp"
#include "AST/Nodes/Declaration.hpp"
#include "AST/Nodes/Declarations/DeclarationsBlock.hpp"
#include "Lexing/Token.hpp"
#include "Semantic/Symbol.hpp"

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

    [[nodiscard]] ModuleSymbol* getSymbolPtr() const override {
        return symbolRef;
    }

    void accept(ASTWalker& walker) override {
        walker.visit(*this);
    }
};

#endif  // VUG_MODULEDECLARATION_HPP

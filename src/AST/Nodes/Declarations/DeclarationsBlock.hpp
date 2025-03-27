// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_DECLARATIONSBLOCK_HPP
#define VUG_DECLARATIONSBLOCK_HPP

#include <utility>

#include "AST/Nodes/Declaration.hpp"

struct DeclarationsBlock : public Declaration {
    std::vector<std::unique_ptr<Declaration>> declarations;

    DeclarationsBlock(std::vector<std::unique_ptr<Declaration>> declarations = {})
        : Declaration(Kind::DeclarationsBlock), declarations(std::move(declarations)) {}

    void evaluate(Evaluator& evaluator) override {
        return evaluator.evaluateDeclaration(*this);
    }
    void accept(ASTWalker& walker) override {
        return walker.visit(*this);
    }
};

#endif//VUG_DECLARATIONSBLOCK_HPP

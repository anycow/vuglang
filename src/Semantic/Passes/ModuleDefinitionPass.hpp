// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_MODULEDEFINITIONPASS_HPP
#define VUG_MODULEDEFINITIONPASS_HPP

#include "AST/ASTWalker.hpp"

class DiagnosticManager;
class SymbolContext;

class ModuleDefinitionPass : public ASTWalker {
   public:
    ModuleDefinitionPass(Node& ast, SymbolContext& context, DiagnosticManager& diagnosticManager)
        : mAst(ast),
          mContext(context),
          mDiagnosticManager(diagnosticManager) {
    }

    void analyze();

    void visit(ModuleDeclaration& node) override;
    void visit(DeclarationsBlock& node) override;
    void visit(FunctionDeclaration& node) override;

   protected:
    Node& mAst;
    SymbolContext& mContext;
    DiagnosticManager& mDiagnosticManager;

    void visit(Node& node) override;
};

#endif  // VUG_MODULEDEFINITIONPASS_HPP

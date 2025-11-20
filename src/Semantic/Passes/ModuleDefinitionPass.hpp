// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_MODULEDEFINITIONPASS_HPP
#define VUG_MODULEDEFINITIONPASS_HPP

#include "AST/ASTWalker.hpp"

class DiagnosticManager;
class SymbolContext;

class ModuleDefinitionPass : public ASTWalker {
public:
    ModuleDefinitionPass(Node& ast,
                         SymbolContext& context,
                         DiagnosticManager& diagnosticManager)
        : _ast(ast),
          _context(context),
          _diagnosticManager(diagnosticManager) {}

    void analyze();

    void visit(ModuleDeclaration& node) override;
    void visit(DeclarationsBlock& node) override;
    void visit(FunctionDeclaration& node) override;

protected:
    Node& _ast;
    SymbolContext& _context;
    DiagnosticManager& _diagnosticManager;

    void visit(Node& node) override;
};

#endif//VUG_MODULEDEFINITIONPASS_HPP

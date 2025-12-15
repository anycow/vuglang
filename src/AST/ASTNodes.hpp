// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_ASTNODES_HPP
#define VUG_ASTNODES_HPP

// For some reason, in clangd, "IWYU pragma: begin_exports" doesn't work properly.

#include "AST/Nodes/Node.hpp"  // IWYU pragma: export

#include "AST/Nodes/Declaration.hpp"                       // IWYU pragma: export
#include "AST/Nodes/Declarations/DeclarationsBlock.hpp"    // IWYU pragma: export
#include "AST/Nodes/Declarations/FunctionDeclaration.hpp"  // IWYU pragma: export
#include "AST/Nodes/Declarations/FunctionParameter.hpp"    // IWYU pragma: export
#include "AST/Nodes/Declarations/ModuleDeclaration.hpp"    // IWYU pragma: export

#include "AST/Nodes/Expression.hpp"                   // IWYU pragma: export
#include "AST/Nodes/Expressions/BinaryOperation.hpp"  // IWYU pragma: export
#include "AST/Nodes/Expressions/CallFunction.hpp"     // IWYU pragma: export
#include "AST/Nodes/Expressions/Identifier.hpp"       // IWYU pragma: export
#include "AST/Nodes/Expressions/Number.hpp"           // IWYU pragma: export
#include "AST/Nodes/Expressions/PrefixOperation.hpp"  // IWYU pragma: export

#include "AST/Nodes/Statement.hpp"                            // IWYU pragma: export
#include "AST/Nodes/Statements/Assign.hpp"                    // IWYU pragma: export
#include "AST/Nodes/Statements/Break.hpp"                     // IWYU pragma: export
#include "AST/Nodes/Statements/ExpressionStatement.hpp"       // IWYU pragma: export
#include "AST/Nodes/Statements/If.hpp"                        // IWYU pragma: export
#include "AST/Nodes/Statements/LocalVariableDeclaration.hpp"  // IWYU pragma: export
#include "AST/Nodes/Statements/Return.hpp"                    // IWYU pragma: export
#include "AST/Nodes/Statements/StatementsBlock.hpp"           // IWYU pragma: export
#include "AST/Nodes/Statements/While.hpp"                     // IWYU pragma: export

#endif  // VUG_ASTNODES_HPP

// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_ASTNODESFORWARD_HPP
#define VUG_ASTNODESFORWARD_HPP

// For some reason, in clangd, "IWYU pragma: begin_exports" doesn't work properly.

struct Node;  // IWYU pragma: export

struct Declaration;          // IWYU pragma: export
struct BadDeclaration;       // IWYU pragma: export
struct DeclarationsBlock;    // IWYU pragma: export
struct FunctionDeclaration;  // IWYU pragma: export
struct FunctionParameter;    // IWYU pragma: export
struct ModuleDeclaration;    // IWYU pragma: export

struct Expression;       // IWYU pragma: export
struct BadExpression;    // IWYU pragma: export
struct BinaryOperation;  // IWYU pragma: export
struct CallFunction;     // IWYU pragma: export
struct Identifier;       // IWYU pragma: export
struct Number;           // IWYU pragma: export
struct PrefixOperation;  // IWYU pragma: export

struct Statement;                 // IWYU pragma: export
struct BadStatement;              // IWYU pragma: export
struct Assign;                    // IWYU pragma: export
struct Break;                     // IWYU pragma: export
struct ExpressionStatement;       // IWYU pragma: export
struct If;                        // IWYU pragma: export
struct LocalVariableDeclaration;  // IWYU pragma: export
struct Return;                    // IWYU pragma: export
struct StatementsBlock;           // IWYU pragma: export
struct While;                     // IWYU pragma: export

#endif  // VUG_ASTNODESFORWARD_HPP

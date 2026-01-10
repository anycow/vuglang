// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_ASTNODESFORWARD_HPP
#define VUG_ASTNODESFORWARD_HPP

// For some reason, in clangd, "IWYU pragma: begin_exports" doesn't work properly.

class Node;  // IWYU pragma: export

class Declaration;          // IWYU pragma: export
class BadDeclaration;       // IWYU pragma: export
class DeclarationsBlock;    // IWYU pragma: export
class FunctionDeclaration;  // IWYU pragma: export
class FunctionParameter;    // IWYU pragma: export
class ModuleDeclaration;    // IWYU pragma: export

class Expression;       // IWYU pragma: export
class BadExpression;    // IWYU pragma: export
class BinaryOperation;  // IWYU pragma: export
class CallFunction;     // IWYU pragma: export
class Identifier;       // IWYU pragma: export
class Number;           // IWYU pragma: export
class PrefixOperation;  // IWYU pragma: export

class Statement;                 // IWYU pragma: export
class BadStatement;              // IWYU pragma: export
class Assign;                    // IWYU pragma: export
class Break;                     // IWYU pragma: export
class ExpressionStatement;       // IWYU pragma: export
class If;                        // IWYU pragma: export
class LocalVariableDeclaration;  // IWYU pragma: export
class Return;                    // IWYU pragma: export
class StatementsBlock;           // IWYU pragma: export
class While;                     // IWYU pragma: export

#endif  // VUG_ASTNODESFORWARD_HPP

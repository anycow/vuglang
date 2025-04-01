// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_STATEMENT_HPP
#define VUG_STATEMENT_HPP

#include "Node.hpp"

#include "Evaluator/Evaluator.hpp"

struct Statement : public Node {
    explicit Statement(Kind nodeType) : Node(nodeType) {}

    bool isStatement() override { return true; }

    virtual StmtResult evaluate(Evaluator& evaluator) { throw std::logic_error("Not implemented"); }
};

#endif//VUG_STATEMENT_HPP

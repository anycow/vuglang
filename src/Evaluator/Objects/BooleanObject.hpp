// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_BOOLEANOBJECT_HPP
#define VUG_BOOLEANOBJECT_HPP

#include "Object.hpp"

class BooleanObject : public Object {
public:
    explicit BooleanObject(bool integerValue)
        : booleanValue(integerValue) {}

    [[nodiscard]] bool getValue() const {
        return booleanValue;
    }

    [[nodiscard]] std::unique_ptr<Object> binaryOperationType(LexemType opType, const Object& rhs) const override {
        switch (opType) {
            case LexemType::LogicAnd:
                return std::make_unique<BooleanObject>(booleanValue &&
                                                       rhs.to<const BooleanObject&>().getValue());
            case LexemType::LogicOr:
                return std::make_unique<BooleanObject>(booleanValue ||
                                                       rhs.to<const BooleanObject&>().getValue());
            default:
                throw std::logic_error("Unsupported operation");
        }
    }
    [[nodiscard]] std::unique_ptr<Object> prefixOperationType(LexemType opType) const override {
        switch (opType) {
            case LexemType::Not:
                return std::make_unique<BooleanObject>(!booleanValue);
            default:
                throw std::logic_error("Unsupported operation");
        }
    }

    std::unique_ptr<Object> clone() override {
        return std::make_unique<BooleanObject>(booleanValue);
    }

    std::string toString() override {
        return std::to_string(booleanValue);
    }

protected:
    bool booleanValue;
};


#endif//VUG_BOOLEANOBJECT_HPP

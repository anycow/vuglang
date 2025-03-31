// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_INTEGEROBJECT_HPP
#define VUG_INTEGEROBJECT_HPP

#include "Object.hpp"

#include "BooleanObject.hpp"

template<typename T>
class IntegerObject : public Object {
public:
    explicit IntegerObject(T integerValue)
        : _integerValue(integerValue) {}

    [[nodiscard]] T getValue() const {
        return _integerValue;
    }

    [[nodiscard]] std::unique_ptr<Object> binaryOperation(LexemType opType, const Object& rhs) const override {
        switch (opType) {
            case LexemType::Equal:
                return std::make_unique<BooleanObject>(
                        _integerValue ==
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Unequal:
                return std::make_unique<BooleanObject>(
                        _integerValue !=
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Less:
                return std::make_unique<BooleanObject>(
                        _integerValue <
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::LessEqual:
                return std::make_unique<BooleanObject>(
                        _integerValue <=
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Greater:
                return std::make_unique<BooleanObject>(
                        _integerValue >
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::GreaterEqual:
                return std::make_unique<BooleanObject>(
                        _integerValue >=
                        rhs.to<const IntegerObject<T>&>().getValue());

            case LexemType::Plus:
                return std::make_unique<IntegerObject<T>>(
                        _integerValue +
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Minus:
                return std::make_unique<IntegerObject<T>>(
                        _integerValue -
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Multiply:
                return std::make_unique<IntegerObject<T>>(
                        _integerValue *
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Divide:
                return std::make_unique<IntegerObject<T>>(
                        _integerValue /
                        rhs.to<const IntegerObject<T>&>().getValue());
            case LexemType::Remainder:
                return std::make_unique<IntegerObject<T>>(
                        _integerValue %
                        rhs.to<const IntegerObject<T>&>().getValue());
            default:
                throw std::logic_error("Unsupported operation");
        }
    }
    [[nodiscard]] std::unique_ptr<Object> prefixOperation(LexemType opType) const override {
        switch (opType) {
            case LexemType::Minus:
                return std::make_unique<IntegerObject>(-_integerValue);
            default:
                throw std::logic_error("Unsupported operation");
        }
    }

    std::unique_ptr<Object> clone() override {
        return std::make_unique<IntegerObject<T>>(_integerValue);
    }

    std::string toString() override {
        return std::to_string(_integerValue);
    }

protected:
    T _integerValue;
};

#endif//VUG_INTEGEROBJECT_HPP

#ifndef VUG_CLI_HPP
#define VUG_CLI_HPP

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Result.hpp"

template <typename T>
struct unwrap_optional {
    using type = T;
};

template <typename T>
struct unwrap_optional<std::optional<T>> {
    using type = T;
};

template <typename T>
using unwrap_optional_t = typename unwrap_optional<T>::type;


template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
inline constexpr bool is_vector_v{is_vector<T>::value};


template <typename T>
struct vector_value_type {
    using type = T;
};

template <typename T, typename Alloc>
struct vector_value_type<std::vector<T, Alloc>> {
    using type = T;
};

template <typename T>
using vector_value_type_t = typename vector_value_type<T>::type;

template <typename T>
inline constexpr bool dependent_false{false};

class CLI {
   public:
    using PriorityType = int32_t;
    static constexpr PriorityType kDefaultOptionPriority = 100;

    enum ParseError {
        MissingOption,
        MissingValue,
        InvalidValue,
        InvalidFormat,
    };
    using ParseResult = Result<void, ParseError>;

    class BaseValue {
       public:
        explicit BaseValue(CLI& cli)
            : mCLI(cli) {
        }
        virtual ~BaseValue() = default;

        BaseValue(const BaseValue& other) = default;
        BaseValue(BaseValue&& other) noexcept = default;
        BaseValue& operator=(const BaseValue& other) = delete;
        BaseValue& operator=(BaseValue&& other) noexcept = delete;

        [[nodiscard]] CLI& getCli() {
            return mCLI;
        }
        [[nodiscard]] const CLI& getCli() const {
            return mCLI;
        }

        [[nodiscard]] PriorityType getPriority() const {
            return mPriority;
        }

       protected:
        void setPriority(const PriorityType priority) {
            mPriority = priority;
        }

       private:
        CLI& mCLI;
        PriorityType mPriority{-1};
    };

    template <typename T>
    class Value : public BaseValue {
       public:
        using UnderlyingType = unwrap_optional_t<T>;
        using ScalarType = vector_value_type_t<unwrap_optional_t<T>>;
        using ParseScalarResult = Result<ScalarType, ParseError>;
        using ParseFunctionType = std::function<ParseScalarResult(const std::string_view)>;

        explicit Value(CLI& cli, T& value)
            : BaseValue(cli),
              mValue(value) {
        }

        void set(const unwrap_optional_t<T>& value, const PriorityType priority) {
            if (priority >= getPriority()) {
                mValue = value;
                setPriority(priority);
            }
        }

        Value<T>& addOption(const std::string& name,
                            const std::optional<UnderlyingType> defaultValue = std::nullopt,
                            std::optional<ParseFunctionType> customParse = std::nullopt,
                            const PriorityType priority = kDefaultOptionPriority) {
            getCli().addOption(
                name,
                std::make_unique<Option<T>>(priority, *this, defaultValue, customParse));
            return *this;
        }

       private:
        T& mValue;
    };

    class BaseOption {
       public:
        explicit BaseOption(const PriorityType priority)
            : mPriority(priority) {
        }
        virtual ~BaseOption() = default;

        BaseOption(const BaseOption&) = default;
        BaseOption& operator=(const BaseOption&) = default;
        BaseOption(BaseOption&&) noexcept = default;
        BaseOption& operator=(BaseOption&&) noexcept = default;

        virtual ParseResult parse(std::optional<std::string_view> argument) = 0;

        [[nodiscard]] PriorityType getPriority() const {
            return mPriority;
        }

       protected:
        void setIsUsed(const bool isUsed) {
            mIsUsed = isUsed;
        }

       private:
        bool mIsUsed{false};
        PriorityType mPriority;
    };

    template <typename T>
    class Option : public BaseOption {
       public:
        using UnderlyingType = typename Value<T>::UnderlyingType;
        using ScalarType = typename Value<T>::ScalarType;
        using ParseScalarResult = typename Value<T>::ParseScalarResult;
        using ParseFunctionType = typename Value<T>::ParseFunctionType;

        explicit Option(const PriorityType priority,
                        Value<T>& value,
                        std::optional<UnderlyingType> defaultValue = std::nullopt,
                        std::optional<ParseFunctionType> customParse = std::nullopt)
            : BaseOption(priority),
              mValue(value),
              mDefaultValue(std::move(defaultValue)) {
            if (customParse) {
                mParseFunction = *customParse;
            } else {
                mParseFunction = parseScalar;
            }
        }

        ParseResult parse(const std::optional<std::string_view> value) override {
            if (!value) {
                if (mDefaultValue) {
                    mValue.set(*mDefaultValue, getPriority());
                    return ParseResult::ok();
                }
                return ParseResult::error(ParseError::MissingValue);
            }

            if constexpr (is_vector_v<UnderlyingType>) {
                std::vector<ScalarType> vector{};

                auto trim = [](auto&& rng) {
                    auto is_space = [](unsigned char c) {
                        return std::isspace(c);
                    };
                    auto start{std::ranges::find_if_not(rng, is_space)};
                    auto end{std::ranges::find_if_not(rng | std::views::reverse, is_space).base()};
                    return std::ranges::subrange(start, end);
                };

                auto scalars = *value
                               | std::views::split(',')
                               | std::views::transform(trim)
                               | std::views::transform([](auto r) {
                                     return std::string_view(r.begin(), r.end());
                                 });

                for (std::string_view element : scalars) {
                    auto scalar{mParseFunction(element)};
                    if (scalar.isError()) {
                        return ParseResult::error(scalar.error());
                    }
                    vector.emplace_back(scalar.value());
                }
                mValue.set(vector, getPriority());
            } else {
                auto scalar{mParseFunction(*value)};
                if (scalar.isError()) {
                    return ParseResult::error(scalar.error());
                }
                mValue.set(scalar.value(), getPriority());
            }
            return ParseResult::ok();
        }

       private:
        Value<T>& mValue;
        std::optional<UnderlyingType> mDefaultValue;
        ParseFunctionType mParseFunction;

        static ParseScalarResult parseScalar(const std::string_view value) {
            if constexpr (std::is_integral_v<ScalarType> && !std::is_same_v<ScalarType, bool>) {
                ScalarType integer{};
                const auto result{
                    std::from_chars(value.data(), value.data() + value.size(), integer)};
                if (result.ec != std::errc{}) {
                    return ParseScalarResult::error(ParseError::InvalidValue);
                }
                return ParseScalarResult::ok(integer);
            } else if constexpr (std::is_same_v<ScalarType, bool>) {
                if (value == "true" || value == "1") {
                    return ParseScalarResult::ok(true);
                } else if (value == "false" || value == "0") {
                    return ParseScalarResult::ok(false);
                } else {
                    return ParseScalarResult::error(ParseError::InvalidValue);
                }
            } else if constexpr (std::is_same_v<ScalarType, std::string>) {
                return ParseScalarResult::ok(std::string(value));
            } else {
                return ParseScalarResult::error(ParseError::InvalidValue);
            }
        }
    };

    template <typename T>
    Value<T>& addValue(T& value) {
        auto val{std::make_unique<Value<T>>(*this, value)};
        auto& ref{*val};
        mValues.push_back(std::move(val));
        return ref;
    }

    void addOption(const std::string& name, std::unique_ptr<BaseOption> option) {
        mOptions.insert({name, std::move(option)});
    }

    ParseResult parse(const int argc, char* argv[]) {  // NOLINT(*-avoid-c-arrays)
        std::vector<std::string_view> args{argv + 1, argv + argc};

        for (size_t i = 0; i < args.size(); ++i) {
            const std::string_view current{args[i]};

            if (current.starts_with("--")) {
                const std::string_view optionStr{current.substr(2)};

                if (const auto pos{optionStr.find('=')}; pos != std::string_view::npos) {
                    const auto key{optionStr.substr(0, pos)};
                    const auto value{optionStr.substr(pos + 1)};

                    const auto option{mOptions.find(std::string(key))};
                    if (option == mOptions.end()) {
                        return ParseResult::error(ParseError::MissingOption);
                    }

                    const auto result{option->second->parse(value)};
                    if (result.isError()) {
                        return result;
                    }
                } else {
                    if (i + 1 < args.size() && !args[i + 1].starts_with("--")) {
                        const auto option{mOptions.find(std::string(optionStr))};
                        if (option == mOptions.end()) {
                            return ParseResult::error(ParseError::MissingOption);
                        }

                        const auto result{option->second->parse(args[i + 1])};
                        i++;
                        if (result.isError()) {
                            return result;
                        }
                    } else {
                        const auto option{mOptions.find(std::string(optionStr))};
                        if (option == mOptions.end()) {
                            return ParseResult::error(ParseError::MissingOption);
                        }

                        const auto result{option->second->parse(std::nullopt)};
                        if (result.isError()) {
                            return result;
                        }
                    }
                }
            } else {
                return ParseResult::error(ParseError::InvalidFormat);
            }
        }

        return ParseResult::ok();
    }

   private:
    std::vector<std::unique_ptr<BaseValue>> mValues;
    std::unordered_map<std::string, std::unique_ptr<BaseOption>> mOptions;
};

#endif

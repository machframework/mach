#pragma once

#include <concepts>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>

#include <mach/detail/core/TypeTraits.hpp>
#include <mach/detail/validation/rules/GeneralRules.hpp>
#include <mach/detail/validation/rules/NumericRules.hpp>
#include <mach/detail/validation/rules/ValidationRuleType.hpp>
#include <mach/detail/validation/rules/StringRules.hpp>
#include <mach/detail/validation/ValidationResult.hpp>
#include <mach/detail/validation/Validator.hpp>

namespace mach
{
    template <typename T>
    class ValidationBuilder;
}

namespace mach::validation
{
    template <typename T, typename Field>
    class FieldValidationBuilder {
    public:
        FieldValidationBuilder& email();
        FieldValidationBuilder& url();

        FieldValidationBuilder& length(std::size_t minLength, std::size_t maxLength);

        FieldValidationBuilder& minLength(std::size_t minLength);
        FieldValidationBuilder& maxLength(std::size_t maxLength);

        FieldValidationBuilder& regex(std::string_view pattern);

        template <detail::traits::Numeric Number>
        FieldValidationBuilder& range(Number min, Number max);

        template <detail::traits::Numeric Number>
        FieldValidationBuilder& min(Number min);

        template <detail::traits::Numeric Number>
        FieldValidationBuilder& max(Number max);

        template <detail::traits::Numeric Number>
        FieldValidationBuilder& multipleOf(Number factor);

        template <typename U>
            requires std::constructible_from<Field, U>
        FieldValidationBuilder& equals(U&& value);

        template <typename U>
            requires std::constructible_from<Field, U>
        FieldValidationBuilder& notEquals(U&& value);

    private:
        FieldValidationBuilder(mach::ValidationBuilder<T>& validationBuilder, Field T::* field)
            : m_validationBuilder(validationBuilder), m_field(field) {}

        template <bool Negate = false, typename Rule>
        FieldValidationBuilder& addRule(Rule rule, std::string_view errorMessage);

        template <detail::traits::Numeric Number>
        static consteval void validateNumericRuleType();

        mach::ValidationBuilder<T>& m_validationBuilder;
        Field T::* m_field;
        std::unordered_set<detail::validation::ValidationRuleType> m_ruleTypes;

        friend class mach::ValidationBuilder<T>;
    };

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::email() {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(
            isString,
            "FileValidationBuild::email() can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(detail::validation::EmailRule{}, "Invalid email format");
        }
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::url() {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(
            isString,
            "FileValidationBuild::url() can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(detail::validation::UrlRule{}, "Invalid URL format");
        }
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::length(
        std::size_t minLength,
        std::size_t maxLength) {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(
            isString,
            "FileValidationBuild::length() can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(
                detail::validation::LengthRule{.minLength = minLength, .maxLength = maxLength},
                "Value is not within the specified length range");
        }
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::minLength(
        std::size_t minLength) {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(isString, "This validation rule can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(
                detail::validation::LengthRule{.minLength = minLength, .maxLength = std::nullopt},
                "Value is too short");
        }
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::maxLength(
        std::size_t maxLength) {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(isString, "This validation rule can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(
                detail::validation::LengthRule{.minLength = std::nullopt, .maxLength = maxLength},
                "Value is too long");
        }
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::regex(
        std::string_view pattern) {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(
            isString,
            "FileValidationBuild::regex() can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(
                detail::validation::RegexRule{.pattern = std::regex(pattern.begin(), pattern.end())},
                "Value does not match the specified regex pattern");
        }
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::range(
        Number min,
        Number max) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::RangeRule<Number>{.min = min, .max = max},
            "Value is not within the specified range");
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::min(Number min) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::RangeRule<Number>{.min = min, .max = std::nullopt},
            "Value is below the specified minimum");
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::max(Number max) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::RangeRule<Number>{.min = std::nullopt, .max = max},
            "Value exceeds the specified maximum");
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::multipleOf(Number factor) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::MultipleOfRule{.factor = factor},
            "Value is not a multiple of the specified factor");
    }

    template <typename T, typename Field>
    template <typename U>
        requires std::constructible_from<Field, U>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::equals(U&& value) {
        return addRule(
            detail::validation::EqualRule<Field>{.value = Field(std::forward<U>(value))},
            "Value is not equal to the specified value");
    }

    template <typename T, typename Field>
    template <typename U>
        requires std::constructible_from<Field, U>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::notEquals(U&& value) {
        return addRule(
            detail::validation::NotEqualRule<Field>{.value = Field(std::forward<U>(value))},
            "Value is equal to the specified value");
    }

    template <typename T, typename Field>
    template <bool Negate, typename Rule>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::addRule(
        Rule rule,
        std::string_view errorMessage) {
        if constexpr (detail::validation::IsUniqueRule<Rule>) {

            const auto [_, inserted] = m_ruleTypes.insert(Rule::Type);
            if (!inserted) {
                throw std::logic_error(
                    "The same validation rule cannot be applied more than once.");
            }
        }
        
        m_validationBuilder.m_validators.emplace_back(
            [field = m_field, rule = std::move(rule), errorMessage = std::string(errorMessage)](
                const T& instance,
                detail::validation::ValidationResult& result) {
                const bool isValid = validate(instance.*field, rule);

                if (isValid == Negate) {
                    result.addError(errorMessage);
                }
            });

        return *this;
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    consteval void FieldValidationBuilder<T, Field>::validateNumericRuleType() {
        static_assert(
            detail::traits::Numeric<Field>,
            "Numeric validation rules can only be applied "
            "to numeric fields.");

        static_assert(
            std::same_as<Number, Field>,
            "Numeric validation arguments must have the same "
            "type as the validated field.");
    }
}
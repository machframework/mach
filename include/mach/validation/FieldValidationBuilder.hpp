#pragma once

#include <concepts>
#include <cstddef>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>

#include <mach/detail/core/TypeTraits.hpp>
#include <mach/detail/validation/rules/GeneralRules.hpp>
#include <mach/detail/validation/rules/NumericRules.hpp>
#include <mach/detail/validation/rules/StringRules.hpp>
#include <mach/detail/validation/rules/ValidationRuleType.hpp>
#include <mach/detail/validation/ValidationResult.hpp>
#include <mach/detail/validation/Validator.hpp>

namespace mach
{
    template <typename T>
    class ValidationBuilder;
}

namespace mach::validation
{
    /**
     * Configures validation rules for a field.
     *
     * Used to associate one or more validation rules with a field of a type.
     *
     * FieldValidationBuilder is returned by ValidationBuilder::field() and is not
     * intended to be instantiated directly.
     *
     * Validation rules are typically configured by chaining calls on the returned
     * builder.
     *
     * @tparam T The type being configured.
     * @tparam Field The type of the field being configured.
     *
     * Thread safety:
     * - Not thread-safe. Validation configuration should be performed from one thread.
     *
     * Stability:
     * - This API is still experimental and may change before Mach's first stable release.
     */
    template <typename T, typename Field>
    class FieldValidationBuilder {

    public:
        /**
         * Applies an email validation rule that requires the field to contain a valid email
         * address.
         *
         * Can only be applied to std::string fields.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        FieldValidationBuilder& email();

        /**
         * Applies a URL validation rule that requires the field to contain a valid URL.
         *
         * Can only be applied to std::string fields.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        FieldValidationBuilder& url();

        /**
         * Applies a length validation rule that requires the field's length to be
         * within the specified range.
         *
         * Can only be applied to std::string fields.
         *
         * @param minLength The minimum allowed length, inclusive.
         * @param maxLength The maximum allowed length, inclusive.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        FieldValidationBuilder& length(std::size_t minLength, std::size_t maxLength);

        /**
         * Applies a minimum length validation rule that requires the field's length
         * to be greater than or equal to the specified value.
         *
         * Can only be applied to std::string fields.
         *
         * @param minLength The minimum allowed length, inclusive.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        FieldValidationBuilder& minLength(std::size_t minLength);

        /**
         * Applies a maximum length validation rule that requires the field's length
         * to be less than or equal to the specified value.
         *
         * Can only be applied to std::string fields.
         *
         * @param maxLength The maximum allowed length, inclusive.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        FieldValidationBuilder& maxLength(std::size_t maxLength);

        /**
         * Applies a regular expression validation rule that requires the field to
         * match the specified pattern.
         *
         * Can only be applied to std::string fields.
         *
         * @param pattern The regular expression pattern to match.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        FieldValidationBuilder& regex(std::string_view pattern);

        /**
         * Applies a range validation rule that requires the field's value to be
         * within the specified range.
         *
         * Can only be applied to numeric fields.
         *
         * @tparam Number The numeric type of the field and range bounds.
         *
         * @param minimum The minimum allowed value, inclusive.
         * @param maximum The maximum allowed value, inclusive.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        template <detail::traits::Numeric Number>
        FieldValidationBuilder& range(Number minimum, Number maximum);

        /**
         * Applies a minimum value validation rule that requires the field's value
         * to be greater than or equal to the specified value.
         *
         * Can only be applied to numeric fields.
         *
         * @tparam Number The numeric type of the field and minimum value.
         *
         * @param minimum The minimum allowed value, inclusive.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        template <detail::traits::Numeric Number>
        FieldValidationBuilder& min(Number minimum);

        /**
         * Applies a maximum value validation rule that requires the field's value
         * to be less than or equal to the specified value.
         *
         * Can only be applied to numeric fields.
         *
         * @tparam Number The numeric type of the field and maximum value.
         *
         * @param maximum The maximum allowed value, inclusive.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        template <detail::traits::Numeric Number>
        FieldValidationBuilder& max(Number maximum);

        /**
         * Applies a multiple-of validation rule that requires the field's value to
         * be a multiple of the specified factor.
         *
         * Can only be applied to numeric fields.
         *
         * @tparam Number The numeric type of the field and factor.
         *
         * @param factor The factor that the field's value must be a multiple of.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        template <detail::traits::Numeric Number>
        FieldValidationBuilder& multipleOf(Number factor);

        /**
         * Applies an equality validation rule that requires the field's value to
         * equal the specified value.
         *
         * @tparam U A type constructible as the field's type.
         *
         * @param value The value to compare against.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        template <typename U>
            requires std::constructible_from<Field, U>
        FieldValidationBuilder& equals(U&& value);

        /**
         * Applies an inequality validation rule that requires the field's value to
         * differ from the specified value.
         *
         * @tparam U A type constructible as the field's type.
         *
         * @param value The value to compare against.
         *
         * @return A reference to this builder for method chaining.
         *
         * Thread safety:
         * - Not thread-safe. Validation configuration should be performed from one thread.
         */
        template <typename U>
            requires std::constructible_from<Field, U>
        FieldValidationBuilder& notEquals(U&& value);

    private:
        FieldValidationBuilder(mach::ValidationBuilder<T>& validationBuilder, Field T::* field);

        template <bool Negate = false, typename Rule>
        FieldValidationBuilder& addRule(Rule rule, std::string_view errorMessage);

        template <detail::traits::Numeric Number>
        static consteval void validateNumericRuleType();

        mach::ValidationBuilder<T>& m_validationBuilder;
        Field T::* m_field;
        std::unordered_set<detail::validation::ValidationRuleType> m_ruleTypes;

        friend class mach::ValidationBuilder<T>;
    };

    //----------------------------------------------------------------
    // Implementation
    //----------------------------------------------------------------

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>::FieldValidationBuilder(
        mach::ValidationBuilder<T>& validationBuilder,
        Field T::* field)
        : m_validationBuilder(validationBuilder), m_field(field) {}

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::email() {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(
            isString,
            "FileValidationBuilder::email() can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(detail::validation::EmailRule{}, "Invalid email format");
        }
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::url() {
        constexpr auto isString = std::same_as<std::remove_cvref_t<Field>, std::string>;
        static_assert(
            isString,
            "FileValidationBuilder::url() can only be used with std::string fields.");

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
            "FileValidationBuilder::length() can only be used with std::string fields.");

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
            "FileValidationBuilder::regex() can only be used with std::string fields.");

        if constexpr (isString) {
            return addRule(
                detail::validation::RegexRule{
                    .pattern = std::regex(pattern.begin(), pattern.end())},
                "Value does not match the specified regex pattern");
        }
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::range(
        Number minimum,
        Number maximum) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::RangeRule<Number>{.min = minimum, .max = maximum},
            "Value is not within the specified range");
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::min(Number minimum) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::RangeRule<Number>{.min = minimum, .max = std::nullopt},
            "Value is below the specified minimum");
    }

    template <typename T, typename Field>
    template <detail::traits::Numeric Number>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::max(Number maximum) {
        validateNumericRuleType<Number>();

        return addRule(
            detail::validation::RangeRule<Number>{.min = std::nullopt, .max = maximum},
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
            m_ruleTypes.insert(Rule::Type);
        }

        m_validationBuilder.m_validators.emplace_back(
            [field = m_field, rule = std::move(rule), errorMessage = std::string(errorMessage)](
                const T& instance,
                detail::validation::ValidationResult& result) {
                if (const bool isValid = detail::validation::validate(instance.*field, rule); isValid == Negate) {
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

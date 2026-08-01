#pragma once

#include <string_view>

#include <mach/detail/validation/rules/GeneralRules.hpp>
#include <mach/detail/validation/rules/NumericRules.hpp>
#include <mach/detail/validation/rules/StringRules.hpp>

namespace mach::detail::validation
{
    bool validate(std::string_view value, const EmailRule& rule);

    bool validate(std::string_view value, const UrlRule& rule);

    template <typename T>
        requires requires(const T& value) {
            { value.size() } -> std::convertible_to<std::size_t>;
        }
    bool validate(const T& value, const LengthRule& rule);

    template <traits::Numeric Number>
    bool validate(const Number& value, const RangeRule<Number>& rule);

    template <traits::Numeric Number>
    bool validate(const Number& value, const MultipleOfRule<Number>& rule);

    template <typename U>
    bool validate(const U& value, const EqualRule<U>& rule);

    template <typename U>
    bool validate(const U& value, const NotEqualRule<U>& rule);

    //----------------------------------------------------------------
    // Implementation
    //----------------------------------------------------------------
    
    template <typename T>
        requires requires(const T& value) {
            { value.size() } -> std::convertible_to<std::size_t>;
        }
    bool validate(const T& value, const LengthRule& rule) {
        if (rule.minLength.has_value() && value.size() < rule.minLength.value()) {
            return false;
        }
        if (rule.maxLength.has_value() && value.size() > rule.maxLength.value()) {
            return false;
        }

        return true;
    }

    template <traits::Numeric Number>
    bool validate(const Number& value, const RangeRule<Number>& rule) {
        if (rule.min.has_value() && value < rule.min.value()) {
            return false;
        }
        if (rule.max.has_value() && value > rule.max.value()) {
            return false;
        }
        return true;
    }

    template <traits::Numeric Number>
    bool validate(const Number& value, const MultipleOfRule<Number>& rule) {
        return value % rule.factor == 0;
    }

    template <typename U>
    bool validate(const U& value, const EqualRule<U>& rule) {
        return value == rule.value;
    }

    template <typename U>
    bool validate(const U& value, const NotEqualRule<U>& rule) {
        return value != rule.value;
    }
}

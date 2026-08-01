#pragma once

#include <optional>
#include <string_view>

#include <mach/detail/validation/rules/StringRules.hpp>
#include <mach/detail/validation/ValidationResult.hpp>
#include <mach/detail/validation/Validator.hpp>

namespace mach
{
    template <typename T>
    class ValidationBuilder;
}

namespace mach::detail::validation
{
    template <typename T, typename Field>
    class FieldValidationBuilder {
        
    public:
        FieldValidationBuilder& email();
        FieldValidationBuilder& url();

        FieldValidationBuilder& length(std::size_t minLength, std::size_t maxLength);
        FieldValidationBuilder& minLength(std::size_t minLength);
        FieldValidationBuilder& maxLength(std::size_t maxLength);


    private:
        FieldValidationBuilder(ValidationBuilder<T>& validationBuilder, Field T::* field)
            : m_validationBuilder(validationBuilder), m_field(field) {}

        template <typename Rule>
        void addRule(const Rule& rule, std::string_view errorMessage);

        ValidationBuilder<T>& m_validationBuilder;
        Field T::* m_field;

        template <typename T>
        friend class ValidationBuilder;
    };

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::email() {
        addRule(EmailRule{}, "Invalid email format");
        return *this;
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::url() {
        addRule(UrlRule{}, "Invalid URL format");
        return *this;
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::length(std::size_t minLength, std::size_t maxLength) {
        addRule(LengthRule{minLength, maxLength}, "Value is not within the specified length range");
        return *this;
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::minLength(std::size_t minLength) {
        addRule(LengthRule{minLength, std::nullopt}, "Value is too short");
        return *this;
    }

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::maxLength(std::size_t maxLength) {
        addRule(LengthRule{std::nullopt, maxLength}, "Value is too long");
        return *this;
    }

    template <typename T, typename Field>
    template <typename Rule>
    void FieldValidationBuilder<T, Field>::addRule(
        const Rule& rule,
        std::string_view errorMessage) {
        m_validationBuilder.m_validators.push_back(
            [field = m_field, rule, errorMessage](const T& instance, ValidationResult& result) {
                if (!validate(instance.*field, rule)) {
                    result.addError(std::string(errorMessage));
                }
            });
    }
}

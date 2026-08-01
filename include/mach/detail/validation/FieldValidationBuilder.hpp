#pragma once

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

    private:
        FieldValidationBuilder(ValidationBuilder<T>& validationBuilder, Field T::* field)
            : m_validationBuilder(validationBuilder), m_field(field) {}

        ValidationBuilder<T>& m_validationBuilder;
        Field T::* m_field;

        template <typename T>
        friend class ValidationBuilder;
    };

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::email() {
        m_validationBuilder.m_validators.push_back(
            [field = m_field](const T& instance, ValidationResult& result) {
                if (!validate(instance.*field, EmailRule{})) {
                    result.addError("Invalid email format");
                }
            });

        return *this;
    }
}

#pragma once

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
        FieldValidationBuilder(ValidationBuilder<T>& validationBuilder, T::Field* field)
            : m_validationBuilder(validationBuilder), m_field(field) {}

        ValidationBuilder<T>& m_validationBuilder;
        T::Field* m_field;

        template <typename T>
        friend class ValidationBuilder;
    };

    template <typename T, typename Field>
    FieldValidationBuilder<T, Field>& FieldValidationBuilder<T, Field>::email() {
        // Add email validation logic here
        return *this;
    }
}

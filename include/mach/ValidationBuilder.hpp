#pragma once

#include <functional>
#include <vector>

#include <mach/detail/validation/FieldValidationBuilder.hpp>
#include <mach/detail/validation/ValidationResult.hpp>

namespace mach::detail::validation
{
    template <typename U, typename Field>
    class FieldValidationBuilder;
}

namespace mach
{
    using detail::validation::FieldValidationBuilder;

	template <typename T>
	class ValidationBuilder {

	public:
        template <typename Field>
        FieldValidationBuilder<T, Field> field(Field T::* field);

    private:
        void validate(const T& instance, detail::validation::ValidationResult& result) const {
            for (const auto& validator : m_validators) {
                validator(instance, result);
            }
        }

        std::vector<std::function<void(const T&, detail::validation::ValidationResult&)>> m_validators;

        template <typename U, typename Field>
        friend class FieldValidationBuilder;
	};

    template <typename T>
    template <typename Field>
    FieldValidationBuilder<T, Field> ValidationBuilder<T>::field(Field T::* field) {
        return FieldValidationBuilder<T, Field>(*this, field);
    }
}

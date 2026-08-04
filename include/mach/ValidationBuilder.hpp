#pragma once

#include <functional>
#include <vector>

#include <mach/detail/validation/ValidationResult.hpp>
#include <mach/validation/FieldValidationBuilder.hpp>

namespace mach::detail::dispatching
{
    template <typename TController, typename TResult, typename... TArgs>
    class ControllerActionInvoker;

    template <typename THandler, typename TResult, typename... TArgs>
    class MinimalApiInvoker;
}

namespace mach
{
	template <typename T>
	class ValidationBuilder {

	public:
        template <typename Field>
        validation::FieldValidationBuilder<T, Field> field(Field T::* field);

    private:
        void validate(const T& instance, detail::validation::ValidationResult& result) const {
            for (const auto& validator : m_validators) {
                validator(instance, result);
            }
        }

        std::vector<std::function<void(const T&, detail::validation::ValidationResult&)>> m_validators;

        template <typename U, typename Field>
        friend class validation::FieldValidationBuilder;

        template <typename TController, typename TResult, typename... TArgs>
        friend class detail::dispatching::ControllerActionInvoker;

        template <typename THandler, typename TResult, typename... TArgs>
        friend class detail::dispatching::MinimalApiInvoker;
	};

    template <typename T>
    template <typename Field>
    validation::FieldValidationBuilder<T, Field> ValidationBuilder<T>::field(Field T::* field) {
        return validation::FieldValidationBuilder<T, Field>(*this, field);
    }
}

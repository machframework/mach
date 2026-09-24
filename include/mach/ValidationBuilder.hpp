#pragma once

#include <functional>
#include <vector>

#include <mach/validation/FieldValidationBuilder.hpp>

#include <mach/detail/validation/ValidationResult.hpp>

namespace mach::detail::dispatching
{
    template <typename TController, typename TResult, typename... TArgs>
    class ControllerActionInvoker;

    template <typename THandler, typename TResult, typename... TArgs>
    class MinimalApiInvoker;
}

namespace mach
{
    /**
     * Configures validation rules for a type.
     *
     * Used to associate validation rules with fields of a type before validation
     * is performed.
     *
     * ValidationBuilder is passed to a type's `validate()` method and is not
     * intended to be instantiated directly.
     *
     * @tparam T The type being configured.
     *
     * Thread safety:
     * - Not thread-safe. Validation configuration should be performed from one thread.
     */
    template <typename T>
    class ValidationBuilder {

    public:
        /**
         * Begins configuring validation rules for a field.
         *
         * Returns a `FieldValidationBuilder` that allows one or more validation
         * rules to be applied to the selected field.
         *
         * Validation rules are typically configured by chaining calls from the
         * returned builder.
         */
        template <typename Field>
        [[nodiscard]] validation::FieldValidationBuilder<T, Field> field(Field T::* field);

    private:
        void validate(const T& instance, detail::validation::ValidationResult& result) const;

        std::vector<std::function<void(const T&, detail::validation::ValidationResult&)>>
            m_validators;

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

    template <typename T>
    void ValidationBuilder<T>::validate(
        const T& instance,
        detail::validation::ValidationResult& result) const {
        for (const auto& validator : m_validators) {
            validator(instance, result);
        }
    }
}

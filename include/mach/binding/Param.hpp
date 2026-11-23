#pragma once

#include <utility>

#include <mach/detail/binding/BindingValueConcept.hpp>
#include <mach/detail/core/FixedString.hpp>

namespace mach::detail::binding
{
    class RequestBinder;
}

namespace mach
{
    /**
     * Represents a route parameter bound to a handler parameter.
     *
     * Provides read-only access to a value extracted from a named route
     * parameter and converted to the specified type.
     *
     * Instances are constructed by Mach during request binding and are
     * not intended to be created directly.
     *
     * @tparam T The type of the value to bind from the route parameter.
     * @tparam Name The compile-time name of the route parameter, matching
     *         a parameter in the route pattern.
     */
    template <detail::binding::BindingValue T, detail::FixedString Name>
    class Param {

    public:
        /**
         * Returns the bound route parameter value.
         *
         * @return A const reference to the bound value.
         */
        [[nodiscard]] const T& value() const noexcept;

        /**
         * Returns the bound route parameter value.
         *
         * @return A const reference to the bound value.
         */
        [[nodiscard]] const T& operator*() const noexcept;

        /**
         * Provides pointer-like access to the bound route parameter value.
         *
         * @return A const pointer to the bound value.
         */
        [[nodiscard]] const T* operator->() const noexcept;

    private:
        explicit Param(T value);

        T m_value;

        friend class detail::binding::RequestBinder;
    };

    template <detail::binding::BindingValue T, detail::FixedString Name>
    const T& Param<T, Name>::value() const noexcept {
        return m_value;
    }

    template <detail::binding::BindingValue T, detail::FixedString Name>
    const T& Param<T, Name>::operator*() const noexcept {
        return m_value;
    }

    template <detail::binding::BindingValue T, detail::FixedString Name>
    const T* Param<T, Name>::operator->() const noexcept {
        return &m_value;
    }

    template <detail::binding::BindingValue T, detail::FixedString Name>
    Param<T, Name>::Param(T value) : m_value(std::move(value)) {}
}

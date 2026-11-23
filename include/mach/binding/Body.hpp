#pragma once

#include <utility>

#include <mach/detail/binding/BindingValueConcept.hpp>

namespace mach::detail::binding
{
    class RequestBinder;
}

namespace mach
{
    /**
     * Represents a request body bound to a handler parameter.
     *
     * Provides read-only access to a value deserialized from the HTTP
     * request body.
     *
     * Instances are constructed by Mach during request binding and are
     * not intended to be created directly.
     *
     * @tparam T The type of the value to bind from the request body.
     */
    template <detail::binding::BindingValue T>
    class Body {

    public:
        /**
         * Returns the bound request body value.
         *
         * @return A const reference to the bound value.
         */
        [[nodiscard]] const T& value() const noexcept;

        /**
         * Returns the bound request body value.
         *
         * @return A const reference to the bound value.
         */
        [[nodiscard]] const T& operator*() const noexcept;

        /**
         * Provides pointer-like access to the bound request body value.
         *
         * @return A const pointer to the bound value.
         */
        [[nodiscard]] const T* operator->() const noexcept;

    private:
        explicit Body(T value);

        T m_value;

        friend class detail::binding::RequestBinder;
    };

    template <detail::binding::BindingValue T>
    const T& Body<T>::value() const noexcept {
        return m_value;
    }

    template <detail::binding::BindingValue T>
    const T& Body<T>::operator*() const noexcept {
        return m_value;
    }

    template <detail::binding::BindingValue T>
    const T* Body<T>::operator->() const noexcept {
        return &m_value;
    }

    template <detail::binding::BindingValue T>
    Body<T>::Body(T value) : m_value(std::move(value)) {}
}

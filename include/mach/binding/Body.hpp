#pragma once

#include <utility>

#include <mach/detail/binding/BindingValueConcept.hpp>

namespace mach::detail::binding
{
    class RequestBinder;
}

namespace mach
{
    template <detail::binding::BindingValue T>
    class Body {

    public:
        [[nodiscard]] const T& value() const noexcept;
        [[nodiscard]] const T& operator*() const noexcept;
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

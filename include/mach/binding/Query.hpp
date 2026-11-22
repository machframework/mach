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
    template <detail::binding::BindingValue T, detail::FixedString Name>
    class Query {

    public:
        [[nodiscard]] const T& value() const noexcept;
        [[nodiscard]] const T& operator*() const noexcept;
        [[nodiscard]] const T* operator->() const noexcept;

    private:
        explicit Query(T value);

        T m_value;

        friend class detail::binding::RequestBinder;
    };

    template <detail::binding::BindingValue T, detail::FixedString Name>
    const T& Query<T, Name>::value() const noexcept {
        return m_value;
    }

    template <detail::binding::BindingValue T, detail::FixedString Name>
    const T& Query<T, Name>::operator*() const noexcept {
        return m_value;
    }

    template <detail::binding::BindingValue T, detail::FixedString Name>
    const T* Query<T, Name>::operator->() const noexcept {
        return &m_value;
    }

    template <detail::binding::BindingValue T, detail::FixedString Name>
    Query<T, Name>::Query(T value) : m_value(std::move(value)) {}
}

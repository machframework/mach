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
        [[nodiscard]] const T& value() const noexcept {
            return m_value;
        }

    private:
        explicit Body(T value) : m_value(std::move(value)) {}

        T m_value;

        friend class detail::binding::RequestBinder;
    };
}

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
        [[nodiscard]] const T& value() const noexcept {
            return m_value;
        }

    private:
        explicit Query(T value) : m_value(std::move(value)) {}

        T m_value;

        friend class detail::binding::RequestBinder;
    };
}

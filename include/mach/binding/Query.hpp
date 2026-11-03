#pragma once

#include <mach/detail/binding/BindingValueConcept.hpp>
#include <mach/detail/core/FixedString.hpp>

namespace mach
{
    template <detail::binding::BindingValue T, detail::FixedString Name>
    class Query {

    private:
        T m_value;
    };
}

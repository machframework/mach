#pragma once

#include <mach/detail/binding/BindingValueConcept.hpp>

namespace mach
{
    template <detail::binding::BindingValue T>
    class Body {

    private:
        T m_value;
    };
}

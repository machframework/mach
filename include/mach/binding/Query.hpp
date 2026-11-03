#pragma once

#include <mach/detail/core/FixedString.hpp>

namespace mach
{
    template <typename T, detail::FixedString Name>
    class Query {

    private:
        T m_value;
    };
}

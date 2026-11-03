#pragma once

#include <mach/detail/binding/BindingTraits.hpp>

namespace mach::detail::binding
{
    template <typename T>
    concept Binding = BindingTraits<T>::is_binding;
}

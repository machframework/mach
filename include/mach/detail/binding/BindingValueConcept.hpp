#pragma once

#include <type_traits>

namespace mach::detail::binding
{
    template <typename T>
    concept BindingValue = std::is_object_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T> &&
                           !std::is_pointer_v<T>;
}

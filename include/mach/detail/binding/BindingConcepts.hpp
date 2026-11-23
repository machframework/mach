#pragma once

#include <concepts>
#include <type_traits>

#include <mach/detail/binding/BindingTraits.hpp>

namespace mach::detail::binding
{
    template <typename T>
    concept Binding = BindingTraits<T>::is_binding;

    template <typename T>
    concept BindingArgument =
        Binding<std::remove_cv_t<std::remove_pointer_t<std::remove_cvref_t<T>>>>;
}

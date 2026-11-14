
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <mach/binding/Body.hpp>
#include <mach/binding/Param.hpp>
#include <mach/binding/Query.hpp>

#include <mach/detail/binding/BindingConcepts.hpp>
#include <mach/detail/binding/BindingTraits.hpp>

namespace
{
    using mach::detail::binding::Binding;
    using mach::detail::binding::BindingSource;
    using mach::detail::binding::BindingTraits;

    // Binding concept

    static_assert(Binding<mach::Body<int>>);
    static_assert(Binding<mach::Param<int, "id">>);
    static_assert(Binding<mach::Query<int, "page">>);

    static_assert(!Binding<int>);
    static_assert(!Binding<std::string>);

    using mach::detail::binding::BindingValue;

    static_assert(BindingValue<int>);
    static_assert(BindingValue<std::string>);
    static_assert(!BindingValue<void>);

    // Construction restrictions

    static_assert(!std::is_default_constructible_v<mach::Body<int>>);
    static_assert(!std::is_default_constructible_v<mach::Param<int, "id">>);
    static_assert(!std::is_default_constructible_v<mach::Query<int, "page">>);

    static_assert(!std::is_constructible_v<mach::Body<int>, int>);
    static_assert(!std::is_constructible_v<mach::Param<int, "id">, int>);
    static_assert(!std::is_constructible_v<mach::Query<int, "page">, int>);

    // Value accessors

    static_assert(
        std::same_as<decltype(std::declval<const mach::Body<int>&>().value()), const int&>);

    static_assert(
        std::same_as<decltype(std::declval<const mach::Param<int, "id">&>().value()), const int&>);

    static_assert(std::same_as<
                  decltype(std::declval<const mach::Query<int, "page">&>().value()),
                  const int&>);

    // Binding traits

    static_assert(BindingTraits<mach::Body<int>>::source == BindingSource::Body);

    static_assert(BindingTraits<mach::Param<int, "id">>::source == BindingSource::Param);

    static_assert(BindingTraits<mach::Query<int, "page">>::source == BindingSource::Query);

    static_assert(std::same_as<typename BindingTraits<mach::Body<int>>::value_type, int>);

    static_assert(std::same_as<typename BindingTraits<mach::Param<int, "id">>::value_type, int>);

    static_assert(std::same_as<typename BindingTraits<mach::Query<int, "page">>::value_type, int>);

    // Binding names

    static_assert(std::string_view(BindingTraits<mach::Param<int, "id">>::name) == "id");
    static_assert(std::string_view(BindingTraits<mach::Query<int, "page">>::name) == "page");
}

int main() {
    return 0;
}

#include <mach/detail/binding/BindingConcepts.hpp>
#include <mach/detail/binding/BindingValueConcept.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

using namespace mach;
using namespace mach::detail::binding;

// Binding recognition

static_assert(Binding<Body<int>>);
static_assert(Binding<Param<int, "id">>);
static_assert(Binding<Query<std::string, "name">>);

static_assert(!Binding<int>);
static_assert(!Binding<std::string>);

// Binding qualifiers

static_assert(!Binding<const Body<int>>);
static_assert(!Binding<Body<int>&>);
static_assert(!Binding<const Body<int>&>);
static_assert(!Binding<Body<int>&&>);

static_assert(!Binding<const Param<int, "id">>);
static_assert(!Binding<Param<int, "id">&>);

static_assert(!Binding<const Query<int, "page">>);
static_assert(!Binding<Query<int, "page">&>);

// Underlying value types

static_assert(std::is_same_v<BindingTraits<Body<int>>::value_type, int>);

static_assert(std::is_same_v<BindingTraits<Param<double, "price">>::value_type, double>);

static_assert(std::is_same_v<BindingTraits<Query<std::string, "search">>::value_type, std::string>);

// Compile-time parameter names

static_assert(std::string_view(BindingTraits<Param<int, "id">>::name.data) == "id");

static_assert(std::string_view(BindingTraits<Query<int, "page">>::name.data) == "page");

// Binding type identity

static_assert(!std::is_same_v<Param<int, "asaf">, Param<int, "hila">>);
static_assert(!std::is_same_v<Query<int, "first">, Query<int, "second">>);
static_assert(std::is_same_v<Param<int, "id">, Param<int, "id">>);
static_assert(!std::is_same_v<Param<int, "id">, Query<int, "id">>);

// Binding value constraints

static_assert(BindingValue<int>);
static_assert(BindingValue<double>);
static_assert(BindingValue<std::string>);
static_assert(BindingValue<std::optional<int>>);
static_assert(BindingValue<std::vector<int>>);

static_assert(!BindingValue<void>);
static_assert(!BindingValue<int*>);
static_assert(!BindingValue<int&>);
static_assert(!BindingValue<int&&>);
static_assert(!BindingValue<const int>);
static_assert(!BindingValue<volatile int>);
static_assert(!BindingValue<const volatile int>);

// Binding class constraints

template <typename T>
concept AcceptsBody = requires { typename Body<T>; };

template <typename T>
concept AcceptsParam = requires { typename Param<T, "id">; };

template <typename T>
concept AcceptsQuery = requires { typename Query<T, "page">; };

static_assert(AcceptsBody<int>);
static_assert(AcceptsBody<std::string>);
static_assert(AcceptsBody<std::optional<int>>);

static_assert(AcceptsParam<int>);
static_assert(AcceptsQuery<std::string>);

static_assert(!AcceptsBody<void>);
static_assert(!AcceptsBody<int*>);
static_assert(!AcceptsBody<int&>);
static_assert(!AcceptsBody<const int>);

static_assert(!AcceptsParam<int*>);
static_assert(!AcceptsParam<int&>);
static_assert(!AcceptsParam<const int>);

static_assert(!AcceptsQuery<int*>);
static_assert(!AcceptsQuery<int&>);
static_assert(!AcceptsQuery<const int>);

int main() {
    return 0;
}

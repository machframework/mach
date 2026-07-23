#include <mach/detail/routing/RouteConstraint.hpp>

#include <charconv>

namespace
{
    bool isInt(std::string_view value) {
        if (value.empty()) {
            return false;
        }

        int result;

        auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

        return ec == std::errc{} && ptr == value.data() + value.size();
    }
}

namespace mach::detail::routing
{
    std::string_view toString(RouteConstraint constraint) {
        switch (constraint) {
        case RouteConstraint::Int:
            return "int";
        }

        return "string";
    }

    std::optional<RouteConstraint> toRouteConstraint(std::string_view value) {
        if (value == "int") {
            return RouteConstraint::Int;
        }

        if (value == "string") {
            return RouteConstraint::String;
        }

        return std::nullopt;
    }

    bool satisfiesConstraint(std::string_view value, RouteConstraint constraint) {
        switch (constraint) {
        case RouteConstraint::Int:
            return isInt(value);
        }

        return false;
    }
}

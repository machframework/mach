#pragma once

#include <concepts>
#include <nlohmann/json.hpp>

namespace mach::detail::binding
{
    template <typename T>
    concept JsonDeserializable = requires(const nlohmann::json& json) {
        { json.template get<T>() } -> std::same_as<T>;
    };
}

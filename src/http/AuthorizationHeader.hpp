#pragma once

#include <optional>
#include <string_view>

namespace mach::detail::http
{
    [[nodiscard]] std::optional<std::string_view> extractAuthorizationScheme(
        std::string_view authorization) noexcept;

    [[nodiscard]] std::optional<std::string_view> extractBearerToken(
        std::string_view authorization) noexcept;
}

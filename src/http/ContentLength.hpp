#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace mach::detail::http
{
    [[nodiscard]] std::optional<std::uint64_t> parseContentLength(std::string_view value) noexcept;
}

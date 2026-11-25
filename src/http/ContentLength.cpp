#include "ContentLength.hpp"
#include "utility/HttpUtils.hpp"

#include <charconv>
#include <system_error>

namespace mach::detail::http
{
    std::optional<std::uint64_t> parseContentLength(std::string_view value) noexcept {
        value = trimOws(value);

        if (value.empty()) {
            return std::nullopt;
        }

        std::uint64_t result{};
        const auto* end = value.data() + value.size();

        const auto [ptr, ec] = std::from_chars(value.data(), end, result, 10);

        if (ec != std::errc{} || ptr != end) {
            return std::nullopt;
        }

        return result;
    }
}

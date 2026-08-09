#include "StringUtils.hpp"

namespace mach::detail
{
    std::vector<std::string_view> split(std::string_view str, char delimiter) {
        std::vector<std::string_view> result;

        std::size_t start = 0;
        std::size_t end = str.find(delimiter);

        while (end != std::string_view::npos) {
            result.emplace_back(trim(str.substr(start, end - start)));

            start = end + 1;
            end = str.find(delimiter, start);
        }

        result.emplace_back(trim(str.substr(start)));

        return result;
    }

    constexpr std::string_view trim(std::string_view str) {
        constexpr auto isWhitespace = [](char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
        };

        while (!str.empty() && isWhitespace(str.front())) {
            str.remove_prefix(1);
        }

        while (!str.empty() && isWhitespace(str.back())) {
            str.remove_suffix(1);
        }

        return str;
    }
}

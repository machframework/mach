#include "AuthorizationHeader.hpp"

#include "utility/HttpUtils.hpp"

namespace
{
    constexpr bool isBearerTokenCharacter(char c) noexcept {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
               c == '-' || c == '.' || c == '_' || c == '~' || c == '+' || c == '/';
    }

    constexpr bool isValidBearerToken(std::string_view value) noexcept {
        if (value.empty() || !isBearerTokenCharacter(value.front())) {
            return false;
        }

        bool paddingStarted = false;

        for (const auto c : value) {
            if (c == '=') {
                paddingStarted = true;
            } else if (paddingStarted || !isBearerTokenCharacter(c)) {
                return false;
            }
        }

        return true;
    }
}

namespace mach::detail::http
{
    std::optional<std::string_view> extractAuthorizationScheme(
        std::string_view authorization) noexcept {
        authorization = trimOws(authorization);

        const auto separator = authorization.find(' ');
        const auto scheme = authorization.substr(0, separator);

        if (!isToken(scheme)) {
            return std::nullopt;
        }

        return scheme;
    }

    std::optional<std::string_view> extractBearerToken(std::string_view authorization) noexcept {
        authorization = trimOws(authorization);

        const auto scheme = extractAuthorizationScheme(authorization);

        if (!scheme || !equalsIgnoreCase(*scheme, "Bearer")) {
            return std::nullopt;
        }

        authorization.remove_prefix(scheme->size());

        while (!authorization.empty() && authorization.front() == ' ') {
            authorization.remove_prefix(1);
        }

        if (!isValidBearerToken(authorization)) {
            return std::nullopt;
        }

        return authorization;
    }
}

#include <mach/detail/http/ContentType.hpp>

namespace
{
    constexpr std::string_view whitespace = " \t";

    [[nodiscard]]
    constexpr std::string_view trim(std::string_view value) noexcept {
        const auto first = value.find_first_not_of(whitespace);

        if (first == std::string_view::npos) {
            return {};
        }

        const auto last = value.find_last_not_of(whitespace);

        return value.substr(first, last - first + 1);
    }

    [[nodiscard]]
    constexpr std::string_view removeQuotes(std::string_view value) noexcept {
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            return value.substr(1, value.size() - 2);
        }

        return value;
    }
}

namespace mach::detail::http
{
    bool matchesMediaType(
        std::string_view contentType,
        std::string_view expectedMediaType) noexcept {
        const auto parametersStart = contentType.find(';');

        const auto mediaType = trim(contentType.substr(0, parametersStart));

        return mediaType == trim(expectedMediaType);
    }

    bool hasUnsupportedCharset(std::string_view contentType) noexcept {
        auto parametersStart = contentType.find(';');

        while (parametersStart != std::string_view::npos) {
            const auto parameterStart = parametersStart + 1;
            const auto nextParameter = contentType.find(';', parameterStart);

            auto parameter = trim(contentType.substr(
                parameterStart,
                nextParameter == std::string_view::npos ? std::string_view::npos
                                                        : nextParameter - parameterStart));

            if (const auto equalsPosition = parameter.find('='); equalsPosition != std::string_view::npos) {
                if (const auto name = trim(parameter.substr(0, equalsPosition)); name == "charset") {
                    auto charset = trim(parameter.substr(equalsPosition + 1));

                    charset = removeQuotes(charset);

                    return charset != "utf-8";
                }
            }

            parametersStart = nextParameter;
        }

        return false;
    }
}

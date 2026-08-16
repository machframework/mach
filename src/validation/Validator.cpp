#include <mach/detail/validation/Validator.hpp>

namespace mach::detail::validation
{
    bool validate(std::string_view value, const EmailRule&) {
        const auto at = value.find('@');
        if (at == std::string_view::npos || at == 0 || at == value.size() - 1) {
            return false;
        }

        if (value.find('@', at + 1) != std::string_view::npos) {
            return false;
        }

        if (const auto dot = value.rfind('.');
            dot == std::string_view::npos || dot <= at + 1 || dot == value.size() - 1) {
            return false;
        }

        return true;
    }

    bool validate(std::string_view value, const UrlRule&) {
        if (value.starts_with("http://")) {
            value.remove_prefix(7);
        } else if (value.starts_with("https://")) {
            value.remove_prefix(8);
        } else {
            return false;
        }

        const auto dot = value.find('.');
        return dot != std::string_view::npos && dot != 0 && dot != value.size() - 1;
    }

    bool validate(std::string_view value, const RegexRule& rule) {
        return std::regex_match(value.begin(), value.end(), rule.pattern);
    }
}

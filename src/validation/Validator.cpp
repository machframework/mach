#include <mach/detail/validation/Validator.hpp>

namespace mach::detail::validation
{
    bool validate(std::string_view value, const EmailRule& rule) {
        // Simple email validation logic (for demonstration purposes)
        auto atPos = value.find('@');
        auto dotPos = value.rfind('.');
        return atPos != std::string_view::npos && dotPos != std::string_view::npos &&
               atPos < dotPos;
    }
    bool validate(std::string_view value, const UrlRule& rule) {
        // Simple URL validation logic (for demonstration purposes)
        return value.starts_with("http://") || value.starts_with("https://");
    }
}
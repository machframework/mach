#include <mach/detail/validation/ValidationResult.hpp>

namespace mach::detail::validation
{
    void ValidationResult::addError(const std::string& error) {
        m_errors.push_back(error);
    }

    bool ValidationResult::hasErrors() const {
        return !m_errors.empty();
    }

    const std::vector<std::string>& ValidationResult::errors() const {
        return m_errors;
    }
}

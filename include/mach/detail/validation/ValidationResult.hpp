#pragma once

#include <string>
#include <vector>

namespace mach::detail::validation
{
    class ValidationResult {
    public:
        void addError(const std::string& error);
        [[nodiscard]] bool hasErrors() const;
        [[nodiscard]] const std::vector<std::string>& errors() const;

    private:
        std::vector<std::string> m_errors;
    };
}
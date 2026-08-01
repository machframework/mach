#pragma once

#include <string>
#include <vector>

namespace mach::detail::validation
{
    class ValidationResult {
    public:
        void addError(const std::string& error);
        bool hasErrors() const;

    private:
        std::vector<std::string> m_errors;
    };
}
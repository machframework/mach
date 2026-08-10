#pragma once

#include <string>

#include <mach/detail/validation/ValidationResult.hpp>

namespace mach::detail::validation
{
    std::string makeValidationError(const ValidationResult& result);
}
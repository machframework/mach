#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <mach/detail/validation/ValidationResult.hpp>

namespace mach::detail::validation
{
    std::string makeValidationError(const mach::detail::validation::ValidationResult& result);
}
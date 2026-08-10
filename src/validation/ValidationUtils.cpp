#include <mach/detail/validation/ValidationUtils.hpp>

#include <nlohmann/json.hpp>

namespace mach::detail::validation
{
    std::string makeValidationError(const ValidationResult& result) {
        nlohmann::json json;
        for (const auto& error : result.errors()) {
            json["errors"].push_back(error);
        }

        return json.dump();
    }
}
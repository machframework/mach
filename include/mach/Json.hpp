#pragma once

#include <nlohmann/json.hpp>

namespace mach
{
	using Json = nlohmann::json;
}

#define MACH_DEFINE_JSON(Type, ...) \
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Type, __VA_ARGS__)

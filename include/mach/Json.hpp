#pragma once

#include <nlohmann/json.hpp>

namespace mach
{
	using Json = nlohmann::json;
}

/**
 * Defines JSON serialization and deserialization functions for a type using
 * nlohmann::json.
 *
 * @warning During deserialization, all listed fields are required by default.
 *			C++ default member initializers are not applied to omitted JSON properties.
 *
 * @param Type The type to serialize.
 * @param ...  The members to serialize and deserialize.
 */
#define MACH_DEFINE_JSON(Type, ...) \
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Type, __VA_ARGS__)

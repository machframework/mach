#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

#include "core/Handler.hpp"
#include "routing/RouteConstraint.hpp"

namespace mach::detail::routing
{
	struct Endpoint {
		mach::http::Method method;
		std::string pattern;
		Handler handler;
		std::unordered_map<std::string, std::optional<RouteConstraint>> parameters;

		bool operator==(const Endpoint& other) const {
			return method == other.method
				&& pattern == other.pattern;
		}
	};

	static inline void hash_combine(size_t& seed, size_t value) {
		seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}

namespace std 
{
	template <>
	struct hash<mach::detail::routing::Endpoint> {
		size_t operator()(const mach::detail::routing::Endpoint& e) const noexcept {
			size_t seed = 0;

			mach::detail::routing::hash_combine(seed, std::hash<mach::http::Method>{}(e.method));
			mach::detail::routing::hash_combine(seed, std::hash<std::string>{}(e.pattern));

			return seed;
		}
	};
}

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

#include <mach/detail/core/MinimalApiHandler.hpp>
#include <mach/detail/dispatching/IEndpointInvoker.hpp>
#include <mach/detail/routing/RouteConstraint.hpp>

namespace mach::detail::routing
{
	enum class EndpointKind {
		MinimalApi,
		ControllerAction
	};

	struct RouteEndpoint {
		mach::http::Method method;
		std::string pattern;
		std::vector<std::string> parameterNames;

		std::unique_ptr<dispatching::IEndpointInvoker> invoker;

		bool operator==(const RouteEndpoint& other) const {
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
	struct hash<mach::detail::routing::RouteEndpoint> {
		size_t operator()(const mach::detail::routing::RouteEndpoint& e) const noexcept {
			size_t seed = 0;

			mach::detail::routing::hash_combine(seed, std::hash<mach::http::Method>{}(e.method));
			mach::detail::routing::hash_combine(seed, std::hash<std::string>{}(e.pattern));

			return seed;
		}
	};
}

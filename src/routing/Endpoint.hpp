#pragma once

#include <string>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

namespace mach::detail::routing
{
	using Handler = void(*)(mach::Context&);

	struct Endpoint {
		mach::http::Method method;
		std::string pattern;
		Handler handler;

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

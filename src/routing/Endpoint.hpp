#pragma once

#include <string>

#include <mach/http/Method.hpp>

namespace mach::detail::routing
{
	using Handler = void(*)(mach::Context&);

	struct Endpoint {
		mach::detail::http::Method method;
		std::string pattern;
		Handler handler;
	};
}

#pragma once

namespace mach::detail::routing
{
	enum class RouteMatchStatus {
		Found,
		NotFound,
		MethodNotAllowed
	};

	constexpr std::string_view toString(RouteMatchStatus status) {
		switch (status) {
		case RouteMatchStatus::Found: return "Found";
		case RouteMatchStatus::NotFound: return "Not Found";
		case RouteMatchStatus::MethodNotAllowed: return "Method Not Allowed";
		default: return "Unknown";
		}
	}
}

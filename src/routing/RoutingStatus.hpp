#pragma once

#include <mach/http/StatusCode.hpp>

namespace mach::detail::routing
{
	enum class RoutingStatus {
		Found,
		NotFound,
		MethodNotAllowed
	};

	constexpr std::string_view toString(RoutingStatus status) {
		switch (status) {
		case RoutingStatus::Found: return "Found";
		case RoutingStatus::NotFound: return "Not Found";
		case RoutingStatus::MethodNotAllowed: return "Method Not Allowed";
		}

		return "Unknown";
	}

	constexpr mach::http::StatusCode toStatusCode(RoutingStatus status) {
		switch (status) {
		case RoutingStatus::Found:
			return mach::http::StatusCode::Ok;
		case RoutingStatus::NotFound:
			return mach::http::StatusCode::NotFound;
		case RoutingStatus::MethodNotAllowed:
			return mach::http::StatusCode::MethodNotAllowed;
		}

		return mach::http::StatusCode::InternalServerError; // fallback for safety
	}
}

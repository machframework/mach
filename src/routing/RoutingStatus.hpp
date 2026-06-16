#pragma once

#include <mach/http/StatusCode.hpp>

namespace mach::detail::routing
{
	using mach::http::StatusCode;

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

	constexpr StatusCode toStatusCode(RoutingStatus status) {
		switch (status) {
		case RoutingStatus::Found:
			return StatusCode::Ok;
		case RoutingStatus::NotFound:
			return StatusCode::NotFound;
		case RoutingStatus::MethodNotAllowed:
			return StatusCode::MethodNotAllowed;
		}

		return mach::http::StatusCode::InternalServerError; // fallback for safety
	}
}

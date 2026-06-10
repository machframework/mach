#pragma once

#include <string_view>

namespace mach::http
{
	enum class Method
	{
		Get,
		Post,
		Put,
		Patch,
		Delete,
		Head,
		Options,
		Unknown
	};

	constexpr std::string_view toString(Method method) {
		switch (method) {
		case Method::Get: return "GET";
		case Method::Post: return "POST";
		case Method::Put: return "PUT";
		case Method::Patch: return "PATCH";
		case Method::Delete: return "DELETE";
		case Method::Head: return "HEAD";
		case Method::Options: return "OPTIONS";
		default: return "UNKNOWN";
		}
	}
}

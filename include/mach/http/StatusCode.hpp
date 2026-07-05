#pragma once

#include <string>

namespace mach::http
{
	enum class StatusCode {
		Ok = 200,
		Created = 201,
		Accepted = 202,
		NonAuthoritativeInformation = 203,
		NoContent = 204,
		PartialContent = 206,

		MovedPermanently = 301,
		MovedTemporarily = 302,
		TemporaryRedirect = 307,
		PermanentRedirect = 308,

		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFound = 404,
		MethodNotAllowed = 405,
		RequestTimeout = 408,
		Conflict = 409,
		PayloadTooLarge = 413,
		UriTooLong = 414,
		RequestHeaderFieldsTooLarge = 431,

		InternalServerError = 500,
		NotImplemented = 501
	};

	constexpr std::string reasonPhrase(StatusCode status)
	{
		switch (status) {
		case StatusCode::Ok:
			return "OK";

		case StatusCode::Created:
			return "Created";

		case StatusCode::BadRequest:
			return "Bad Request";

		case StatusCode::NotFound:
			return "Not Found";

		case StatusCode::MethodNotAllowed:
			return "Method Not Allowed";

		case StatusCode::PayloadTooLarge:
			return "Payload Too Large";

		case StatusCode::UriTooLong:
			return "URI Too Long";

		case StatusCode::RequestHeaderFieldsTooLarge:
			return "Request Header Fields Too Large";

		case StatusCode::InternalServerError:
			return "Internal Server Error";
		
		case StatusCode::NotImplemented:
			return "Not Implemented";
		}

		return "Unknown Status";
	}
}

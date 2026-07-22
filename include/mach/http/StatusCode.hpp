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
		Found = 302,
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
		UnsupportedMediaType = 415,
		RequestHeaderFieldsTooLarge = 431,

		InternalServerError = 500,
		NotImplemented = 501,
		HttpVersionNotSupported = 505
	};

	constexpr std::string_view reasonPhrase(StatusCode status)
	{
		switch (status) {
		case StatusCode::Ok:
			return "OK";

		case StatusCode::Created:
			return "Created";

		case StatusCode::Accepted:
			return "Accepted";

		case StatusCode::NonAuthoritativeInformation:
			return "Non-Authoritative Information";

		case StatusCode::NoContent:
			return "No Content";

		case StatusCode::PartialContent:
			return "Partial Content";

		case StatusCode::MovedPermanently:
			return "Moved Permanently";

		case StatusCode::Found:
			return "Found";

		case StatusCode::TemporaryRedirect:
			return "Temporary Redirect";

		case StatusCode::PermanentRedirect:
			return "Permanent Redirect";

		case StatusCode::BadRequest:
			return "Bad Request";

		case StatusCode::Unauthorized:
			return "Unauthorized";

		case StatusCode::Forbidden:
			return "Forbidden";

		case StatusCode::NotFound:
			return "Not Found";

		case StatusCode::MethodNotAllowed:
			return "Method Not Allowed";

		case StatusCode::RequestTimeout:
			return "Request Timeout";

		case StatusCode::Conflict:
			return "Conflict";

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

		case StatusCode::HttpVersionNotSupported:
			return "HTTP Version Not Supported";
		}

		return "Unknown Status";
	}

	constexpr bool isValidStatusCode(StatusCode code)
	{
		switch (static_cast<StatusCode>(code)) {
		case StatusCode::Ok:
		case StatusCode::Created:
		case StatusCode::Accepted:
		case StatusCode::NonAuthoritativeInformation:
		case StatusCode::NoContent:
		case StatusCode::PartialContent:

		case StatusCode::MovedPermanently:
		case StatusCode::Found:
		case StatusCode::TemporaryRedirect:
		case StatusCode::PermanentRedirect:

		case StatusCode::BadRequest:
		case StatusCode::Unauthorized:
		case StatusCode::Forbidden:
		case StatusCode::NotFound:
		case StatusCode::MethodNotAllowed:
		case StatusCode::RequestTimeout:
		case StatusCode::Conflict:
		case StatusCode::PayloadTooLarge:
		case StatusCode::UriTooLong:
		case StatusCode::UnsupportedMediaType:
		case StatusCode::RequestHeaderFieldsTooLarge:

		case StatusCode::InternalServerError:
		case StatusCode::NotImplemented:
		case StatusCode::HttpVersionNotSupported:
			return true;

		default:
			return false;
		}
	}
}

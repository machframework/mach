#include <string_view>

namespace mach::http
{
	enum class Method
	{
		GET,
		POST,
		PUT,
		PATCH,
		DELETE,
		HEAD,
		OPTIONS,
		UNKNOWN
	};

	constexpr std::string_view to_string(Method method) {
		switch (method) {
		case Method::GET: return "GET";
		case Method::POST: return "POST";
		case Method::PUT: return "PUT";
		case Method::PATCH: return "PATCH";
		case Method::DELETE: return "DELETE";
		case Method::HEAD: return "HEAD";
		case Method::OPTIONS: return "OPTIONS";
		default: return "UNKNOWN";
		}
	}
}

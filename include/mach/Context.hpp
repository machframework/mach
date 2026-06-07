#pragma once

#include <string>
#include <unordered_map>

#include <mach/Request.hpp>
#include <mach/Response.hpp>

namespace mach
{
	/**
	 * Represents an HTTP context used by the server during the request-handling process.
	 *
	 * This object provides access to HTTP request, response and request parameters
	 *
	 * Lifetime:
	 * - The Context object is valid only during the request handling scope.
	 *
	 * Thread safety:
	 * - This class is not thread-safe. Concurrent access must be synchronized externally.
	 *
	 * Ownership:
	 * - The Context object is owned by the framework and should not be stored
	 *   beyond the request handling lifetime.
	 */
	struct Context {
		Request request;
		Response response;
		std::unordered_map<std::string, std::string> params;
	};
}
#pragma once

#include <utility>

#include <mach/Request.hpp>
#include <mach/Response.hpp>

#include <mach/detail/application/ExecutionPlan.hpp>

namespace mach::detail::dispatching 
{
	class Dispatcher;
}

namespace mach
{
	/**
	 * Represents the per-request HTTP context used during request handling.
	 *
	 * Provides access to the current HTTP request, response, and route parameters.
	 *
	 * Lifetime:
	 * - Valid only for the duration of the request handling scope.
	 *
	 * Ownership:
	 * - Owned by the framework.
	 * - Must not be copied, moved, or stored beyond the request handling lifetime.
	 *
	 * Thread safety:
	 * - Not thread-safe. Concurrent access must be synchronized externally.
	 */
	struct Context {
		Context(Request&& request, Response&& response) 
			: request(std::move(request)),
			response(std::move(response))
		{ }

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;

		Request request;
		Response response;

		friend class detail::dispatching::Dispatcher;
	private:
		detail::application::ExecutionPlan executionPlan;
	};
}

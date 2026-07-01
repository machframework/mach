#pragma once

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

#include <mach/detail/routing/Router.hpp>

namespace mach::detail::routing
{
	class RoutingMiddleware {

	public:
		explicit RoutingMiddleware(Router& router)
			: m_router(router)
		{ }

		void invoke(mach::Context& context, mach::Next next) {
			// route

			next();
		}
	private:
		Router& m_router;
	};
}

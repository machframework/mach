#include <mach/detail/routing/RoutingMiddleware.hpp>

namespace mach::detail::routing
{
	void RoutingMiddleware::invoke(mach::Context& context, mach::Next next) {
		auto plan = m_router.route(context.request);

		next();
	}
}

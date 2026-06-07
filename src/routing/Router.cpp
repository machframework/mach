#include "Router.hpp"

namespace mach::detail::routing
{
	mach::detail::application::ExecutionPlan Router::route(const mach::Request& request) const {		
		return mach::detail::application::ExecutionPlan{};
	}

	void Router::addRoute(Endpoint&& route) {
	}

	Endpoint* Router::matchRoute(const mach::Request& request) const {
		return nullptr;
	}
}

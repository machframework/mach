#pragma once

#include <mach/Context.hpp>

#include "dispatching/Dispatcher.hpp"
#include "routing/Endpoint.hpp"
#include "routing/Router.hpp"

namespace mach::detail::application
{
	class Runtime {

	public:
		void handle(mach::Context& context);
		void addRoute(routing::Endpoint&& route);

	private:
		routing::Router m_router;
		dispatching::Dispatcher m_dispatcher;
	};
}

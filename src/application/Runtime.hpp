#pragma once

#include <mach/Context.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/routing/Endpoint.hpp>

#include "dispatching/Dispatcher.hpp"
#include "routing/Router.hpp"

namespace mach::detail::application
{
	class Runtime {

	public:
		Runtime(routing::Router router, di::Container container);

		void handle(mach::Context& context);
		void addRoute(routing::Endpoint&& endpoint);

	private:
		routing::Router m_router;
		di::Container m_container;
		dispatching::Dispatcher m_dispatcher;
	};
}

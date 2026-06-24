#pragma once

#include <mach/Context.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/routing/Router.hpp>

#include "dispatching/Dispatcher.hpp"

namespace mach::detail::application
{
	class Runtime {

	public:
		Runtime(const Runtime&) = delete;
		Runtime& operator=(const Runtime&) = delete;
		Runtime(Runtime&&) = delete;
		Runtime& operator=(Runtime&&) = delete;

		Runtime(routing::Router router, di::Container container);

		void handle(mach::Context& context);
		void addRoute(routing::RouteEndpoint&& endpoint);

	private:
		routing::Router m_router;
		di::Container m_container;
		dispatching::Dispatcher m_dispatcher;
	};
}

#pragma once

#include <string_view>

#include "application/Runtime.hpp"
#include "routing/Router.hpp"
#include "server/Server.hpp"

namespace mach
{
	namespace application = mach::detail::application;
	namespace routing = mach::detail::routing;
	namespace server = mach::detail::server;

	class App {

	public:
		App(std::string_view host, std::uint16_t port, std::size_t threadCount = 1);

		void run();

	private:
		server::Server m_server;
		application::Runtime m_runtime;
		routing::Router m_router;
	};
}

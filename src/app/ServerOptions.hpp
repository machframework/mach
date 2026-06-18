#pragma once

#include <string>

namespace mach::detail::app
{
	struct ServerOptions {
		std::string host;
		uint16_t port;
		std::size_t threads;
	};
}

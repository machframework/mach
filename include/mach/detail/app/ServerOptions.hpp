#pragma once

#include <string>
#include <string_view>

namespace mach::detail::app
{
	struct ServerOptions {
		std::string host;
		uint16_t port;
		std::size_t threads;
	};
}

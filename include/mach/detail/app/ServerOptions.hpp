#pragma once

#include <string>
#include <string_view>

namespace mach::detail::app
{
	struct ServerOptions {
		std::string host;
		std::int32_t port;
		std::int64_t threads;
	};
}

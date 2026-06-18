#include <mach/AppBuilder.hpp>

namespace mach
{
	AppBuilder::AppBuilder(std::string_view host, std::uint16_t port, std::size_t threadCount) {
		m_serverOptions = detail::app::ServerOptions{
			.host = std::string(host),
			.port = port,
			.threads = threadCount
		};
	}
}

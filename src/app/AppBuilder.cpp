#include <mach/AppBuilder.hpp>

namespace mach
{
	AppBuilder::AppBuilder(std::string_view host, std::uint16_t port, std::size_t threadCount) {
		m_serverOptions = detail::app::ServerOptions{
			std::string(host),
			port, threadCount
		};
	}

	App AppBuilder::build() {
		return mach::App(std::move(m_serverOptions), std::move(m_container));
	}
}

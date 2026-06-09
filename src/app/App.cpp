#include <mach/App.hpp>

namespace mach
{
	App::App(std::string_view host, std::uint16_t port, std::size_t threadCount)
		: m_server(host, port, threadCount, m_runtime)
	{ }

	void App::run() {
		m_server.run();
	}
}

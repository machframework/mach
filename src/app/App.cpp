#include <mach/App.hpp>

#include <string>

#include "application/Runtime.hpp"
#include "server/Server.hpp"

namespace mach
{
	class App::Impl {
	
	public: 
		Impl(std::string_view host, std::uint16_t port, std::size_t threadCount);
		~Impl() = default;

		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void run();

	private:
		detail::server::Server m_server;
		detail::application::Runtime m_runtime;
	};

	App::App(std::string_view host, std::uint16_t port, std::size_t threadCount)
		: m_impl(std::make_unique<Impl>(host, port, threadCount))
	{ }

	App::~App() = default;

	std::string App::host() const noexcept {
		return m_impl->host();
	}

	std::uint16_t App::port() const noexcept {
		return m_impl->port();
	}

	std::size_t App::threadCount() const noexcept {
		return m_impl->threadCount();
	}

	void App::run() {
		m_impl->run();
	}

	App::Impl::Impl(std::string_view host, std::uint16_t port, std::size_t threadCount) 
		: m_server(host, port, threadCount, m_runtime)
	{ }

	void App::Impl::run() {
		m_server.run();
	}

	std::string App::Impl::host() const noexcept {
		return m_server.host();
	}

	std::uint16_t App::Impl::port() const noexcept {
		return m_server.port();
	}

	std::size_t App::Impl::threadCount() const noexcept {
		return m_server.threadCount();
	}
}

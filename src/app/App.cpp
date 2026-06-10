#include <mach/App.hpp>

#include <string>

#include "application/Runtime.hpp"
#include "routing/Endpoint.hpp"
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

		void addRoute(mach::http::Method method, std::string&& pattern, detail::Handler handler);

		void run();

	private:
		detail::server::Server m_server;
		detail::application::Runtime m_runtime;
	};

	App::App(std::string_view host, std::uint16_t port, std::size_t threadCount)
		: m_impl(std::make_unique<Impl>(host, port, threadCount))
	{ }

	App::~App() = default;

	void App::run() {
		m_impl->run();
	}

	void App::addRoute(mach::http::Method method, std::string pattern, detail::Handler handler) {
		m_impl->addRoute(method, std::move(pattern), handler);
	}

	std::string App::host() const noexcept {
		return m_impl->host();
	}

	std::uint16_t App::port() const noexcept {
		return m_impl->port();
	}

	std::size_t App::threadCount() const noexcept {
		return m_impl->threadCount();
	}

	App::Impl::Impl(std::string_view host, std::uint16_t port, std::size_t threadCount) 
		: m_server(host, port, threadCount, m_runtime)
	{ }

	void App::Impl::run() {
		m_server.run();
	}

	void App::Impl::addRoute(mach::http::Method method, std::string&& pattern, detail::Handler handler) {
		mach::detail::routing::Endpoint endpoint{
			.method = method,
			.pattern = std::move(pattern),
			.handler = handler
		};

		m_runtime.addRoute(std::move(endpoint));
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

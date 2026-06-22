#include <mach/App.hpp>

#include <string>
#include <stdexcept>

#include <mach/detail/routing/RouteEndpoint.hpp>

#include "application/Runtime.hpp"
#include "server/Server.hpp"

namespace mach
{
	class App::Impl {
	
	public: 
		Impl(detail::app::ServerOptions serverOptions);

		~Impl() = default;

		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void addRoute(mach::http::Method method, std::string_view pattern, detail::MinimalApiHandler handler);

		void run();

	private:
		detail::app::ServerOptions m_serverOptions;

		//detail::application::Runtime m_runtime;
		detail::routing::Router m_router;
		detail::di::Container m_container;
	};

	App::App(
		detail::app::ServerOptions serverOptions,
		detail::di::Container container
	)
		: m_impl(std::make_unique<Impl>(serverOptions))
	{ }

	App::~App() = default;

	void App::run() {
		m_impl->run();
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

	void App::addRouteImpl(http::Method method, std::string_view pattern, detail::MinimalApiHandler handler) {
		m_impl->addRoute(method, pattern, handler);
	}

	App::Impl::Impl(detail::app::ServerOptions serverOptions)
		: m_serverOptions(std::move(serverOptions))
		//: m_server(std::move(serverOptions), std::move(m_runtime))
	{ }

	void App::Impl::run() {
		auto runtime = detail::application::Runtime(
			std::move(m_router),
			std::move(m_container)
		);

		auto server = std::make_unique<detail::server::Server>(
			std::move(m_serverOptions),
			std::move(runtime)
		);

		server->run();
	}

	void App::Impl::addRoute(mach::http::Method method, std::string_view pattern, detail::MinimalApiHandler handler) {
		if (!handler) {
			throw std::invalid_argument("Route handler cannot be empty");
		}
		
		mach::detail::routing::RouteEndpoint endpoint{
			.method = method,
			.pattern = std::string(pattern),
			.handler = handler
		};

		m_router.addRoute(std::move(endpoint));
	}

	std::string App::Impl::host() const noexcept {
		return m_serverOptions.host;
	}

	std::uint16_t App::Impl::port() const noexcept {
		return m_serverOptions.port;
	}

	std::size_t App::Impl::threadCount() const noexcept {
		return m_serverOptions.threads;
	}
}

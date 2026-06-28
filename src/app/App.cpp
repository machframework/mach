#include <mach/App.hpp>

#include <string>
#include <stdexcept>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/dispatching/MinimalApiInvoker.hpp>

#include "server/Server.hpp"

namespace mach
{
	class App::Impl {
	
	public: 
		Impl(
			detail::app::ServerOptions serverOptions,
			detail::di::Container container,
			detail::middleware::MiddlewarePipeline middlewarePipeline
		);

		~Impl() = default;

		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void addRoute(detail::routing::RouteEndpoint route);
		void addControllerRoutes(std::vector<detail::routing::RouteEndpoint> routes);

		void run();

	private:
		detail::app::ServerOptions m_serverOptions;

		detail::routing::Router m_router;
		detail::di::Container m_container;
		detail::middleware::MiddlewarePipeline m_middlewarePipeline;
	};

	App::App(
		detail::app::ServerOptions serverOptions,
		detail::di::Container container,
		detail::middleware::MiddlewarePipeline middlewarePipeline
	)
		: m_impl(std::make_unique<Impl>(
			std::move(serverOptions),
			std::move(container),
			std::move(middlewarePipeline)
		))
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

	void App::addRouteImpl(
		detail::routing::RouteEndpoint route
	) {
		m_impl->addRoute(std::move(route));
	}

	void App::addControllerRoutesImpl(std::vector<detail::routing::RouteEndpoint> routes) {
		m_impl->addControllerRoutes(std::move(routes));
	}

	App::Impl::Impl(
		detail::app::ServerOptions serverOptions,
		detail::di::Container container,
		detail::middleware::MiddlewarePipeline middlewarePipeline
	)
		: m_serverOptions(std::move(serverOptions)),
		m_container(std::move(container)),
		m_middlewarePipeline(std::move(middlewarePipeline))
	{ }

	void App::Impl::run() {
		auto server = std::make_unique<detail::server::Server>(
			std::move(m_serverOptions),
			std::move(m_router),
			std::move(m_container),
			std::move(m_middlewarePipeline)
		);

		server->run();
	}

	void App::Impl::addRoute(detail::routing::RouteEndpoint route) {
		if (!route.invoker) {
			throw std::invalid_argument("Route handler cannot be empty");
		}

		m_router.addRoute(std::move(route));
	}

	void App::Impl::addControllerRoutes(std::vector<detail::routing::RouteEndpoint> routes) {
		for (auto& route : routes) {
			m_router.addRoute(std::move(route));
		}
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

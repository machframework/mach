#include <mach/App.hpp>

#include <iostream>
#include <string>
#include <stdexcept>
#include <typeindex>
#include <unordered_set>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/dispatching/MinimalApiInvoker.hpp>

#include "server/Server.hpp"

namespace
{
	enum class AppState {
		Ready,
		Running,
		Stopped
	};
}

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
		void addControllerRoutes(std::vector<detail::routing::RouteEndpoint> routes, std::type_index controllerType);

		int run();
		void stop();

	private:
		detail::app::ServerOptions m_serverOptions;
		AppState m_state = AppState::Ready;
		std::unordered_set<std::type_index> m_mappedControllers;

		std::mutex m_serverMutex;
		std::unique_ptr<detail::server::Server> m_server;

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

	App::App(App&&) noexcept = default;

	int App::run() {
		return m_impl->run();
	}

	void App::stop() {
		m_impl->stop();
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

	void App::addControllerRoutesImpl(std::vector<detail::routing::RouteEndpoint> routes, std::type_index controllerType) {
		m_impl->addControllerRoutes(std::move(routes), controllerType);
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

	int App::Impl::run()
	{
		detail::server::Server* server = nullptr;

		{
			std::lock_guard lock(m_serverMutex);

			if (m_state == AppState::Running) {
				throw std::logic_error(
					"The application is already running"
				);
			}

			if (m_state != AppState::Ready) {
				throw std::logic_error(
					"The application has already run"
				);
			}

			// This is redundant if state is authoritative.
			if (m_server) {
				throw std::logic_error(
					"The application is already running"
				);
			}

			m_container.addSingletonInstance<detail::routing::Router>(
				std::move(m_router)
			);

			m_container.finalizeRegistrations();

			m_server =
				std::make_unique<detail::server::Server>(
					std::move(m_serverOptions),
					std::move(m_container),
					std::move(m_middlewarePipeline)
				);

			server = m_server.get();
			m_state = AppState::Running;
		}

		int result = 0;

		try {
			server->run();
		}
		catch (const std::exception& exception) {
			std::cout
				<< "Mach error: "
				<< exception.what()
				<< '\n';

			result = 1;
		}
		catch (...) {
			std::cout
				<< "Mach error: unknown server failure\n";

			result = 1;
		}

		{
			std::lock_guard lock(m_serverMutex);

			m_server.reset();
			m_state = AppState::Stopped;
		}

		return result;
	}

	void App::Impl::stop() {
		detail::server::Server* server = nullptr;
		
		{
			std::lock_guard lock(m_serverMutex);
			server = m_server.get();

			if (server && m_state == AppState::Running) {
				server->stop();
				m_state = AppState::Stopped;
			}
		}
	}

	void App::Impl::addRoute(detail::routing::RouteEndpoint route) {
		if (!route.invoker) {
			throw std::invalid_argument("Route handler cannot be empty");
		}

		m_router.addRoute(std::move(route));
	}

	void App::Impl::addControllerRoutes(std::vector<detail::routing::RouteEndpoint> routes, std::type_index controllerType) {
		const auto [_, inserted] =
			m_mappedControllers.emplace(controllerType);

		if (!inserted) {
			throw std::logic_error(
				"Mach error: controller '" +
				std::string(controllerType.name()) +
				"' has already been mapped"
			);
		}

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

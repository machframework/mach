#include <mach/AppBuilder.hpp>

#include <iostream>
#include <stdexcept>

#include <mach/logging/Logging.hpp>

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/routing/Router.hpp>
#include <mach/detail/routing/RoutingMiddleware.hpp>

namespace
{
	void validateHost(std::string_view host) {
		if (host.empty()) {
			throw std::invalid_argument("Host cannot be empty.");
		}
		if (std::ranges::any_of(host, [](unsigned char c) {
			return std::iscntrl(c) || std::isspace(c);
			})) {
			throw std::invalid_argument("Host cannot contain whitespace or control characters.");
		}
	}

	void validatePort(std::int32_t port) {
		if (port < 1 || port > 65535) {
			throw std::invalid_argument("Invalid port. Port must be between 1 and 65535.");
		}
	}

	void validateThreadCount(std::int64_t threadCount) {
		if (threadCount < 1) {
			throw std::invalid_argument("Thread count must be at least 1.");
		}
	}
}

namespace mach
{
	AppBuilder::AppBuilder(std::string_view host, std::int32_t port, std::int64_t threadCount) {
		m_serverOptions = detail::app::ServerOptions{
			std::string(host),
			port, threadCount
		};

		// register preprocessing middleware

		// register routing middleware
		this->use<detail::routing::RoutingMiddleware, detail::routing::Router>();
	}

	App AppBuilder::build(){
		try {
			validateHost(m_serverOptions.host);
			validatePort(m_serverOptions.port);
			validateThreadCount(m_serverOptions.threads);
		}
		catch (const std::exception& ex) {
			mach::detail::logging::Logger::error("Failed to build Mach application: " + std::string(ex.what()) + "\n");
			throw;
		}
		catch (...) {
			mach::detail::logging::Logger::error("Failed to build Mach application: Unknown exception\n");
			throw;
		}
		
		m_container.addService<detail::binding::BodyBinder>(detail::di::ServiceLifetime::Singleton);

		return mach::App(
			std::move(m_serverOptions),
			std::move(m_container),
			std::move(m_middlewarePipeline)
		);
	}
}

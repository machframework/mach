#include "Server.hpp"

#include <format>
#include <thread>
#include <vector>

#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>

#include <mach/logging/Logging.hpp>

namespace mach::detail::server
{
	Server::Server(
		app::ServerOptions serverOptions,
		routing::Router router,
		di::Container container,
		middleware::MiddlewarePipeline middlewarePipeline
	)
		: m_threadCount(serverOptions.threads),
		m_endpoint(boost::asio::ip::make_address(serverOptions.host), static_cast<std::uint16_t>(serverOptions.port)),
		m_ioc(static_cast<int>(serverOptions.threads)),
		m_runtime(std::move(router), std::move(container), std::move(middlewarePipeline))
	{}

	std::string Server::host() const noexcept {
		return m_endpoint.address().to_string();
	}

	std::uint16_t Server::port() const noexcept {
		return m_endpoint.port();
	}

	std::size_t Server::threadCount() const noexcept {
		return m_threadCount;
	}

	void Server::run() {
		m_listener = std::make_shared<BeastListener>(
			m_ioc,
			m_endpoint,
			m_runtime,
			m_requestAdapter,
			m_responseAdapter
		);

		net::co_spawn(
			m_ioc,
			m_listener->run(),
			net::detached
		);

		// Run the I/O service on the requested number of threads
		std::vector <std::thread> threads;
		threads.reserve(m_threadCount - 1);

		detail::logging::Logger::info(std::format("Starting Mach server on {}:{} with {} threads", host(), port(), m_threadCount));

		for (int i = 0; i < m_threadCount - 1; ++i) {
			threads.emplace_back(
				[this]
				{
					m_ioc.run();
				}
			);
		}

		// Run main thread
		m_ioc.run();

		// Join threadss
		for (auto& thread : threads) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}
}

#include <mach/server/Server.hpp>

#include <format>
#include <thread>
#include <vector>

#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

#include <mach/logging/Logging.hpp>

#include "adapter/inbound/BeastRequestAdapter.hpp"
#include "adapter/outbound/BeastResponseAdapter.hpp"
#include "BeastListener.hpp"
#include "BeastSession.hpp"
#include "application/Runtime.hpp"

namespace mach::detail::server
{
	namespace net = boost::asio;

	class Server::Impl {
	
	public:
		Impl(const boost::asio::ip::address& address, std::uint16_t port, std::size_t threadCount);
		~Impl() = default;

		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void run();

	private:
		std::size_t m_thread_count;
		boost::asio::ip::tcp::endpoint m_endpoint;
		boost::asio::io_context m_ioc;
		std::shared_ptr<BeastListener> m_listener;

		detail::application::Runtime m_runtime;
		detail::http::adapter::BeastRequestAdapter m_requestAdapter;
		detail::http::adapter::BeastResponseAdapter m_responseAdapter;
	};

	Server::Server(const std::string& host, std::uint16_t port, std::size_t threadCount)
		: m_impl(std::make_unique<Impl>(boost::asio::ip::make_address(host), port, threadCount))
	{}

	Server::~Server() = default;

	std::string Server::host() const noexcept {
		return m_impl->host();
	}

	std::uint16_t Server::port() const noexcept {
		return m_impl->port();
	}

	std::size_t Server::threadCount() const noexcept {
		return m_impl->threadCount();
	}

	void Server::run() {
		m_impl->run();
	}

	Server::Impl::Impl(const boost::asio::ip::address& address, std::uint16_t port, std::size_t threadCount)
		: m_thread_count(threadCount), 
		m_endpoint(address, port), 
		m_ioc(static_cast<int>(threadCount))
	{}

	std::string Server::Impl::host() const noexcept {
		return m_endpoint.address().to_string();
	}

	std::uint16_t Server::Impl::port() const noexcept {
		return m_endpoint.port();
	}

	std::size_t Server::Impl::threadCount() const noexcept {
		return m_thread_count;
	}

	void Server::Impl::run() {
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
		threads.reserve(m_thread_count - 1);

		detail::logging::Logger::info(std::format("Starting Mach server on {}:{} with {} threads", host(), port(), m_thread_count));

		for (int i = 0; i < m_thread_count - 1; ++i) {
			threads.emplace_back(
				[this]
				{
					m_ioc.run();
				}
			);
		}

		// Run main thread
		m_ioc.run();
	}
}

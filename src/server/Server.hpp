#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

#include <mach/detail/app/ServerOptions.hpp>
#include <mach/detail/routing/Router.hpp>
#include <mach/detail/di/Container.hpp>

#include "adapter/inbound/BeastRequestAdapter.hpp"
#include "adapter/outbound/BeastResponseAdapter.hpp"
#include "BeastListener.hpp"
#include "BeastSession.hpp"

namespace mach::detail::server
{
	namespace net = boost::asio;

	class Server {

	public:
		Server(
			app::ServerOptions serverOptions,
			routing::Router router,
			di::Container container
		);

		~Server() = default;
		
		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void run();

	private:
		boost::asio::ip::tcp::endpoint m_endpoint;
		boost::asio::io_context m_ioc;
		std::shared_ptr<BeastListener> m_listener;
		std::size_t m_threadCount;

		detail::application::Runtime m_runtime;
		detail::http::adapter::BeastRequestAdapter m_requestAdapter;
		detail::http::adapter::BeastResponseAdapter m_responseAdapter;
	};
}

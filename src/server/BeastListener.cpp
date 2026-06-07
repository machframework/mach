//
// Derived from Boost.Beast examples.
//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include "BeastListener.hpp"

#include <memory>
#include <string>
#include <utility>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "BeastSession.hpp"
#include <mach/logging/Logging.hpp>

namespace mach::detail::server
{
	using detail::logging::Logger;

    BeastListener::BeastListener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        detail::application::Runtime& runtime,
        detail::http::adapter::BeastRequestAdapter& requestAdapter,
        detail::http::adapter::BeastResponseAdapter& responseAdapter
    )
        : m_ioc(ioc),
        m_acceptor(net::make_strand(ioc)),
        m_runtime(runtime),
        m_requestAdapter(requestAdapter),
        m_responseAdapter(responseAdapter)
    {
        beast::error_code ec;

        // Open the acceptor
        m_acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            Logger::error("Failed to open acceptor");
            return;
        }

        // Allow address reuse
        m_acceptor.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            Logger::error("Failed to set socket option");
            return;
        }

        // Bind to the server address
        m_acceptor.bind(endpoint, ec);
        if (ec) {
            Logger::error("Failed to bind acceptor");
            return;
        }

        // Start listening for connections
        m_acceptor.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            Logger::error("Failed to listen");
            return;
        }
    }

    // Start accepting incoming connections
    net::awaitable<void> BeastListener::run() {
        co_await do_accept();
    }

    net::awaitable<void> BeastListener::do_accept()
    {
		beast::error_code ec;

        // The new connection gets its own strand
        tcp::socket socket = co_await m_acceptor.async_accept(
            net::make_strand(m_ioc), net::redirect_error(net::use_awaitable, ec)
        );

        if (ec) {
            Logger::error("Failed to accept connection");
            co_return; // To avoid infinite loop
        } 

		auto& executor = socket.get_executor();

        // Create the session and run it
        auto session = std::make_shared<BeastSession>(
            std::move(socket),
            m_runtime,
            m_requestAdapter,
            m_responseAdapter
        );

        net::co_spawn(
            executor,
            [session]() -> net::awaitable<void>
            {
                co_await session->run();
            }(),
            net::detached
        );

        // Accept another connection in the same session
        co_await do_accept();
    }
}

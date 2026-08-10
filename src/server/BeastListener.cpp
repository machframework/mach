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

#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <mach/AppOptions.hpp>
#include <mach/Logger.hpp>

#include "BeastSession.hpp"

namespace mach::detail::server
{
    BeastListener::BeastListener(
        const AppOptions& appOptions,
        net::io_context& ioc,
        const tcp::endpoint& endpoint,
        application::Runtime& runtime,
        detail::http::adapter::BeastRequestAdapter& requestAdapter,
        detail::http::adapter::BeastResponseAdapter& responseAdapter,
        const mach::Logger& logger)
        : m_ioc(ioc), m_acceptor(net::make_strand(ioc)), m_appOptions(appOptions),
          m_runtime(runtime), m_requestAdapter(requestAdapter), m_responseAdapter(responseAdapter),
          m_logger(logger) {
        beast::error_code ec;

        // Open the acceptor
        m_acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            throw std::runtime_error("Failed to open acceptor: " + ec.message());
        }

#ifdef _WIN32

         constexpr BOOL exclusiveAddressUse = TRUE;

        if (::setsockopt(
                m_acceptor.native_handle(),
                SOL_SOCKET,
                SO_EXCLUSIVEADDRUSE,
                reinterpret_cast<const char*>(&exclusiveAddressUse),
                sizeof(exclusiveAddressUse)) == SOCKET_ERROR) {
            ec.assign(::WSAGetLastError(), boost::system::system_category());
        }

#else

        m_acceptor.set_option(net::socket_base::reuse_address(true), ec);

#endif

        if (ec) {
            throw std::runtime_error("Failed to configure socket options: " + ec.message());
        }

        // Bind to the server address
        m_acceptor.bind(endpoint, ec);
        if (ec) {
            throw std::runtime_error(
                std::format(
                    "Failed to bind to {}:{}: {}",
                    endpoint.address().to_string(),
                    endpoint.port(),
                    ec.message()));
        }

        // Start listening for connections
        m_acceptor.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            throw std::runtime_error(
                "Failed to start listening for incoming requests: " + ec.message());
        }
    }

    // Start accepting incoming connections
    net::awaitable<void> BeastListener::run() {
        co_await do_accept();
    }

    void BeastListener::stop() {
        boost::system::error_code ec;

        // cancel pending async operations
        m_acceptor.cancel(ec);
        m_acceptor.close(ec);
    }

    net::awaitable<void> BeastListener::do_accept() {
        while (true) {
            beast::error_code ec;

            tcp::socket socket = co_await m_acceptor.async_accept(
                net::make_strand(m_ioc),
                net::redirect_error(net::use_awaitable, ec));

            if (ec == net::error::operation_aborted || ec == net::error::bad_descriptor ||
                ec == net::error::not_socket) {
                co_return;
            }

            if (ec) {
                m_logger.error("Failed to accept connection: {}", ec.message());
                continue;
            }

            auto executor = socket.get_executor();

            auto session = std::make_shared<BeastSession>(
                m_appOptions,
                std::move(socket),
                m_runtime,
                m_requestAdapter,
                m_responseAdapter,
                m_logger);

            net::co_spawn(
                m_ioc,
                [session]() -> net::awaitable<void> {
                    co_await session->run();
                },
                net::detached);
        }
    }
}

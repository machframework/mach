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

#include "BeastSession.hpp"
#include "Logging.hpp"

namespace mach::detail::server
{
    BeastListener::BeastListener(net::io_context& ioc, tcp::endpoint endpoint)
        : m_ioc(ioc), m_acceptor(net::make_strand(ioc))
    {
        beast::error_code ec;

        // Open the acceptor
        m_acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            mach::detail::logging::error("Failed to open acceptor");
            return;
        }

        // Allow address reuse
        m_acceptor.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            mach::detail::logging::error("Failed to set socket option");
            return;
        }

        // Bind to the server address
        m_acceptor.bind(endpoint, ec);
        if (ec) {
            mach::detail::logging::error("Failed to bind acceptor");
            return;
        }

        // Start listening for connections
        m_acceptor.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            mach::detail::logging::error("Failed to listen");
            return;
        }
    }

    // Start accepting incoming connections
    void BeastListener::run() {
        do_accept();
    }

    void BeastListener::do_accept()
    {
        // The new connection gets its own strand
        m_acceptor.async_accept(net::make_strand(m_ioc),
            beast::bind_front_handler(
                &BeastListener::on_accept,
                shared_from_this())
        );
    }

    void BeastListener::on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec) {
            //mach::logging::fail(ec, "accept");
            return; // To avoid infinite loop
        }
        else {
            // Create the session and run it
            std::make_shared<BeastSession>(
                std::move(socket)
            )->run();
        }

        // Accept another connection in the same session
        do_accept();
    }
}

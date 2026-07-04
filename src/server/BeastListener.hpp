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

#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/http.hpp>

#include "application/Runtime.hpp"
#include "adapter/inbound/BeastRequestAdapter.hpp"

namespace mach::detail::server
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http = beast::http;           // from <boost/beast/http.hpp>
    namespace net = boost::asio;            // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

    // Accepts incoming connections and launches the sessions
    class BeastListener : public std::enable_shared_from_this<BeastListener>
    {
        net::io_context& m_ioc;
        tcp::acceptor m_acceptor;

    public:
        BeastListener(
            net::io_context& ioc,
            tcp::endpoint endpoint,
            detail::application::Runtime& runtime,
            detail::http::adapter::BeastRequestAdapter& requestAdapter,
            detail::http::adapter::BeastResponseAdapter& responseAdapter
        );

        // Start accepting incoming connections
        net::awaitable<void> run();
        void stop();

    private:
        net::awaitable<void> do_accept();

        detail::application::Runtime& m_runtime;
        detail::http::adapter::BeastRequestAdapter& m_requestAdapter;
        detail::http::adapter::BeastResponseAdapter& m_responseAdapter;
    };
}

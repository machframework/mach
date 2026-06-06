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

#include "BeastSession.hpp"

//
#include <iostream>
//

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/core/ignore_unused.hpp>

namespace mach::detail::server
{
    // Take ownership of the stream
    BeastSession::BeastSession(
        tcp::socket socket,
        detail::application::Runtime& runtime,
        detail::http::adapter::BeastRequestAdapter& requestAdapter,
        detail::http::adapter::BeastResponseAdapter& responseAdapter
    )
        : m_stream(std::move(socket)),
        m_runtime(runtime),
        m_requestAdapter(requestAdapter),
        m_responseAdapter(responseAdapter)
    {}

    // Start the asynchronous operation
    net::awaitable<void> BeastSession::run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.

        co_await net::dispatch(m_stream.get_executor(), net::use_awaitable);
		co_await do_read();

        co_return;
    }

    net::awaitable<void> BeastSession::do_read() {
        // Make the request empty before reading,
        // otherwise the operation behavior is undefined.
        m_req = {};

        // Set the timeout.
        m_stream.expires_after(std::chrono::seconds(30));

		beast::error_code ec;

        // Read a request
        co_await http::async_read(
            m_stream,
            m_buffer,
            m_req,
            net::redirect_error(net::use_awaitable, ec)
        );

        // This means they closed the connection
        if (ec == http::error::end_of_stream ||
            ec == net::error::eof ||
            ec == net::error::connection_reset ||
            ec == net::error::connection_aborted ||
            ec == beast::error::timeout)
        {
            do_close();
            co_return;
        }

        if (ec) {
            Logger::error(std::format("Failed to read request: {}", ec.message()));
			Logger::error(std::format("Method was '{}', target was '{}'", std::string( m_req.method_string()), std::string( m_req.target())));
            co_return;
        }

        // Send the response
        co_await send_response(handle_request(std::move(m_req)));
    }

    net::awaitable<void> BeastSession::send_response(http::message_generator&& msg) {
        bool keep_alive = msg.keep_alive();

        beast::error_code ec;

        // Write the response
        co_await beast::async_write(
            m_stream,
            std::move(msg),
            net::redirect_error(net::use_awaitable, ec)
        );

        if (ec) {
            Logger::error(std::format("Failed to write response: {}", ec.message()));
            co_return;
        }

        if (!keep_alive) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            do_close();
            co_return;
        }

        // Read another request
        co_await do_read();
    }

    void BeastSession::do_close() {
        // Send a TCP shutdown
        beast::error_code ec;
        m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
}

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

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include <boost/core/ignore_unused.hpp>

namespace mach::detail::server
{
    // Take ownership of the stream
    BeastSession::BeastSession(
        tcp::socket&& socket)
        : m_stream(std::move(socket))
    {}

    // Start the asynchronous operation
    void BeastSession::run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(m_stream.get_executor(),
            beast::bind_front_handler(
                &BeastSession::do_read,
                shared_from_this())
        );
    }

    void BeastSession::do_read() {
        // Make the request empty before reading,
        // otherwise the operation behavior is undefined.
        m_req = {};

        // Set the timeout.
        m_stream.expires_after(std::chrono::seconds(30));

        // Read a request
        http::async_read(m_stream, m_buffer, m_req,
            beast::bind_front_handler(
                &BeastSession::on_read,
                shared_from_this())
        );
    }

    void BeastSession::on_read(
        beast::error_code ec,
        std::size_t bytes_transferred) 
    {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if (ec == http::error::end_of_stream ||
            ec == net::error::eof ||
            ec == net::error::connection_reset ||
            ec == net::error::connection_aborted ||
            ec == beast::error::timeout)
        {
            return do_close();
        }

        if (ec) {
            return Logger::error(std::format("Failed to read request: {}", ec.message()));
        }

        // Send the response
        send_response(handle_request(std::move(m_req)));
    }

    void BeastSession::send_response(http::message_generator&& msg) {
        bool keep_alive = msg.keep_alive();

        // Write the response
        beast::async_write(
            m_stream,
            std::move(msg),
            beast::bind_front_handler(
                &BeastSession::on_write, shared_from_this(), keep_alive)
        );
    }

    void BeastSession::on_write(
        bool keep_alive,
        beast::error_code ec,
        std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
            return Logger::error(std::format("Failed to write response: {}", ec.message()));
        }

        if (!keep_alive) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // Read another request
        do_read();
    }

    void BeastSession::do_close() {
        // Send a TCP shutdown
        beast::error_code ec;
        m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
}

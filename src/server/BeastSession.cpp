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
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/message_generator.hpp>

#include <mach/http/StatusCode.hpp>
#include <mach/http/Version.hpp>

namespace
{
    namespace http = boost::beast::http;

    http::message_generator makeReadErrorResponse(mach::http::StatusCode status)
    {
        http::response<http::string_body> res;
        
        constexpr unsigned http11Version = 11;
        res.version(http11Version);
        res.result(static_cast<unsigned int>(status));
        res.set(http::field::server, "Mach");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(false);
        res.body() = mach::http::reasonPhrase(status);

        res.prepare_payload();
        return res;
    }
}
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
    { }

    // Start the asynchronous operation
    net::awaitable<void> BeastSession::run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.

        co_await net::dispatch(
            m_stream.get_executor(),
            net::use_awaitable
        );

        while (true) {
            const bool keepAlive = co_await do_read();

            if (!keepAlive) {
                do_close();
                co_return;
            }
        }
    }

    net::awaitable<bool> BeastSession::do_read() {
        // Set the read timeout
        m_stream.expires_after(std::chrono::seconds(30));
		beast::error_code ec;

        http::request_parser<http::string_body> parser;
        parser.body_limit(1024 * 1024);
        parser.header_limit(16 * 1024);
        constexpr std::size_t maxTargetSize = 8 * 1024;

        // Read a request
        co_await http::async_read(
            m_stream,
            m_buffer,
            parser,
            net::redirect_error(net::use_awaitable, ec)
        );

        if (ec == http::error::end_of_stream ||
            ec == net::error::eof ||
            ec == net::error::connection_reset ||
            ec == net::error::connection_aborted ||
            ec == net::error::operation_aborted ||
            ec == beast::error::timeout)
        {
            co_return false;
        }

        if (ec == http::error::body_limit) {
            co_return co_await send_response(makeReadErrorResponse(mach::http::StatusCode::PayloadTooLarge));
        }
        else if (ec == http::error::header_limit) {
            co_return co_await send_response(makeReadErrorResponse(mach::http::StatusCode::RequestHeaderFieldsTooLarge));
        }
        else if (ec) {
            Logger::warning(std::format("Failed to read request: {}", ec.message()));
            co_return false;
        }

        auto req = parser.release();

        if (req.target().size() > maxTargetSize) {
            co_return co_await send_response(makeReadErrorResponse(mach::http::StatusCode::UriTooLong));
        }

        // Send the response
        co_return co_await send_response(handle_request(std::move(req)));
    }

    net::awaitable<bool> BeastSession::send_response(http::message_generator&& msg) {
        const bool keep_alive = msg.keep_alive();

        // Set the write timeout
        m_stream.expires_after(std::chrono::seconds(30));
        beast::error_code ec;

        // Write the response
        co_await beast::async_write(
            m_stream,
            std::move(msg),
            net::redirect_error(net::use_awaitable, ec)
        );

        if (ec) {
            Logger::warning(std::format("Failed to write response: {}", ec.message()));
            co_return false;
        }

        co_return keep_alive;
    }

    void BeastSession::do_close() {
        // Send a TCP shutdown
        beast::error_code ec;
        m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
}

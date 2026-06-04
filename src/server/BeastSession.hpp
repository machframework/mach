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

#include <format>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/message.hpp>

#include <mach/logging/Logging.hpp>

namespace mach::detail::server
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http = beast::http;           // from <boost/beast/http.hpp>
    namespace net = boost::asio;            // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

    using mach::detail::logging::Logger;

    // Handles an HTTP server connection
    class BeastSession : public std::enable_shared_from_this<BeastSession> {
        beast::tcp_stream m_stream;
        beast::flat_buffer m_buffer;
        http::request<http::string_body> m_req;

    public:
        BeastSession(tcp::socket socket);

        // Start the asynchronous operation
        net::awaitable<void> run();

        net::awaitable<void> do_read();

        net::awaitable<void> send_response(http::message_generator&& msg);

        void do_close();

    private:
        // Return a response for the given request.
        // The concrete type of the response message (which depends on the
        // request), is type-erased in message_generator.
        template <typename Body, typename Allocator>
        http::message_generator handle_request(
            http::request<Body, http::basic_fields<Allocator>>&& req);
    };

    template <typename Body, typename Allocator>
    http::message_generator BeastSession::handle_request(
        http::request<Body, http::basic_fields<Allocator>>&& req) 
    {
        Logger::info(std::format("Received request: {} {}", req.method_string(), req.target()));

		auto req_body = req.body();

        http::response<http::string_body> res{
            http::status::ok,
            req.version()
        };

        res.set(http::field::server, "Mach");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());

		res.body() = req_body; // echo the request body back in the response
        res.prepare_payload();

        return res;
    }
}

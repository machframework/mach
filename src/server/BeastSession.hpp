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

#include <atomic>
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

#include <mach/Context.hpp>
#include <mach/logging/Logging.hpp>

#include "adapter/inbound/BeastRequestAdapter.hpp"
#include "adapter/outbound/BeastResponseAdapter.hpp"
#include "application/Runtime.hpp"

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
        BeastSession(
            tcp::socket socket,
            detail::application::Runtime& runtime,
            detail::http::adapter::BeastRequestAdapter& requestAdapter,
            detail::http::adapter::BeastResponseAdapter& responseAdapter
        );

        ~BeastSession() {
			s_aliveSessions--;
        }

        static std::int64_t aliveCount() {
			return s_aliveSessions.load();
        }

		static std::int64_t createdCount() {
			return s_createdSessions.load();
		}

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

        detail::application::Runtime& m_runtime;
        detail::http::adapter::BeastRequestAdapter& m_requestAdapter;
        detail::http::adapter::BeastResponseAdapter& m_responseAdapter;

		static inline std::atomic<std::int64_t> s_createdSessions = 0;
		static inline std::atomic<std::int64_t> s_aliveSessions = 0;
    };

    template <typename Body, typename Allocator>
    http::message_generator BeastSession::handle_request(
        http::request<Body, http::basic_fields<Allocator>>&& req) 
    {
        bool keepAlive = req.keep_alive();
        auto version = req.version();

        auto context = m_requestAdapter.adapt(std::move(req));
     
        Logger::info(std::format("Received request: {}", context.request.target()));

        m_runtime.handle(context);
        
        auto res = m_responseAdapter.adapt(std::move(context));

        res.set(http::field::server, "Mach");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(keepAlive);

        res.prepare_payload();

        return res;
    }
}

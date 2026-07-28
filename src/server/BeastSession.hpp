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
#include <optional>
#include <utility>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/parser.hpp>

#include <mach/Context.hpp>
#include <mach/Logger.hpp>

#include "adapter/inbound/BeastRequestAdapter.hpp"
#include "adapter/outbound/BeastResponseAdapter.hpp"
#include "application/Runtime.hpp"

namespace mach::detail::server
{
    namespace beast = boost::beast;   // from <boost/beast.hpp>
    namespace http = beast::http;     // from <boost/beast/http.hpp>
    namespace net = boost::asio;      // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

    // Handles an HTTP server connection
    class BeastSession : public std::enable_shared_from_this<BeastSession> {
        beast::tcp_stream m_stream;
        beast::flat_buffer m_buffer;

    public:
        BeastSession(
            tcp::socket socket,
            detail::application::Runtime& runtime,
            detail::http::adapter::BeastRequestAdapter& requestAdapter,
            detail::http::adapter::BeastResponseAdapter& responseAdapter,
            const mach::Logger& logger);

        // Start the asynchronous operation
        net::awaitable<void> run();

        net::awaitable<bool> do_read();

        net::awaitable<bool> send_response(http::message_generator&& msg);

        void do_close();

    private:
        // Return a response for the given request.
        // The concrete type of the response message (which depends on the
        // request), is type-erased in message_generator.
        template <typename Body, typename Allocator>
        http::message_generator handle_request(
            http::request<Body, http::basic_fields<Allocator>>&& req);

        http::message_generator makeReadErrorResponse(mach::http::StatusCode status);

        detail::application::Runtime& m_runtime;
        detail::http::adapter::BeastRequestAdapter& m_requestAdapter;
        detail::http::adapter::BeastResponseAdapter& m_responseAdapter;

        const mach::Logger& m_logger;
    };

    template <typename Body, typename Allocator>
    http::message_generator BeastSession::handle_request(
        http::request<Body, http::basic_fields<Allocator>>&& req) {
        bool keepAlive = req.keep_alive();
        auto version = req.version();

        bool adapterRejectedRequest = false;

        try {
            auto context = m_requestAdapter.adapt(std::move(req), adapterRejectedRequest);
            auto method = context.request.method();

#ifndef NDEBUG
            m_logger.info("Received request: {}", context.request.target());
#endif

            if (!adapterRejectedRequest) {
                m_runtime.handle(context);
            }

            auto res = m_responseAdapter.adapt(std::move(context));

            res.set(http::field::server, "Mach");
            res.set(http::field::content_type, "text/plain");
            res.keep_alive(keepAlive);

            if (res.result() == http::status::no_content ||
                res.result() == http::status::not_modified) {
                res.body().clear();
            }

            res.prepare_payload();

            if (method == mach::http::Method::Head) {
                const auto bodySize = res.body().size();

                res.body().clear();
                res.content_length(bodySize);
            }

            return res;
        } catch (const std::exception& ex) {
            m_logger.error("Request handling failed: {}", ex.what());
            return makeReadErrorResponse(mach::http::StatusCode::InternalServerError);
        } catch (...) {
            m_logger.error("Request handling failed with unknown exception");
            return makeReadErrorResponse(mach::http::StatusCode::InternalServerError);
        }
    }
}

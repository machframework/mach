#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

#include <mach/ServerOptions.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/middleware/MiddlewarePipeline.hpp>
#include <mach/detail/routing/Router.hpp>

#include "BeastListener.hpp"
#include "BeastSession.hpp"
#include "adapter/inbound/BeastRequestAdapter.hpp"
#include "adapter/outbound/BeastResponseAdapter.hpp"

namespace mach::detail::server
{
    namespace net = boost::asio;

    class Server {

    public:
        Server(
            ServerOptions serverOptions,
            di::Container container,
            middleware::MiddlewarePipeline middlewarePipeline);

        ~Server() = default;

        std::string host() const noexcept;
        std::uint16_t port() const noexcept;
        std::size_t threadCount() const noexcept;

        void run();
        void stop();

    private:
        boost::asio::ip::tcp::endpoint m_endpoint;
        boost::asio::io_context m_ioc;
        std::shared_ptr<BeastListener> m_listener;
        std::size_t m_threadCount;

        detail::application::Runtime m_runtime;
        detail::http::adapter::BeastRequestAdapter m_requestAdapter;
        detail::http::adapter::BeastResponseAdapter m_responseAdapter;
    };
}

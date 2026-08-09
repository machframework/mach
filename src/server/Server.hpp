#pragma once

#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

#include <mach/AppOptions.hpp>
#include <mach/Logger.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/middleware/MiddlewarePipeline.hpp>

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
            AppOptions&& appOptions,
            di::Container container,
            middleware::MiddlewarePipeline middlewarePipeline,
            const mach::Logger& logger);

        ~Server() = default;

        std::string host() const noexcept;
        std::uint16_t port() const noexcept;
        std::size_t threadCount() const noexcept;

        void run();
        void stop();

    private:
        AppOptions m_appOptions;

        boost::asio::ip::tcp::endpoint m_endpoint;
        boost::asio::io_context m_ioc;
        std::shared_ptr<BeastListener> m_listener;

        detail::application::Runtime m_runtime;
        detail::http::adapter::BeastRequestAdapter m_requestAdapter;
        detail::http::adapter::BeastResponseAdapter m_responseAdapter;

        const mach::Logger& m_logger;
    };
}

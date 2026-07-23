#include "Server.hpp"

#include <exception>
#include <format>
#include <mutex>
#include <thread>
#include <vector>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>

#include <mach/logging/Logging.hpp>

namespace mach::detail::server
{
    Server::Server(
        ServerOptions serverOptions,
        di::Container container,
        middleware::MiddlewarePipeline middlewarePipeline)
        : m_threadCount(serverOptions.threadCount),
          m_endpoint(
              boost::asio::ip::make_address(serverOptions.host),
              static_cast<std::uint16_t>(serverOptions.port)),
          m_ioc(static_cast<int>(serverOptions.threadCount)),
          m_runtime(std::move(container), std::move(middlewarePipeline)) {}

    std::string Server::host() const noexcept {
        return m_endpoint.address().to_string();
    }

    std::uint16_t Server::port() const noexcept {
        return m_endpoint.port();
    }

    std::size_t Server::threadCount() const noexcept {
        return m_threadCount;
    }

    void Server::run() {
        m_listener = std::make_shared<BeastListener>(
            m_ioc,
            m_endpoint,
            m_runtime,
            m_requestAdapter,
            m_responseAdapter);

        // configure signals
        net::signal_set signals(m_ioc, SIGINT, SIGTERM);
        signals.async_wait([this](boost::system::error_code ec, int signal) {
            if (!ec) {
                stop();
            }
        });

        net::co_spawn(m_ioc, m_listener->run(), net::detached);

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> threads;
        threads.reserve(m_threadCount - 1);

        detail::logging::Logger::info(
            std::format(
                "Starting Mach server on {}:{} with {} threads",
                host(),
                port(),
                m_threadCount));

        std::mutex exceptionMutex;
        std::exception_ptr iocException = nullptr;

        auto runIoContext = [&] {
            try {
                m_ioc.run();
            } catch (...) {
                std::lock_guard lock(exceptionMutex);
                if (!iocException) {
                    iocException = std::current_exception();
                }

                m_ioc.stop();
            }
        };

        for (int i = 0; i < m_threadCount - 1; ++i) {
            threads.emplace_back(runIoContext);
        }

        // Run main thread
        runIoContext();

        // Join threads
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        if (iocException) {
            std::rethrow_exception(iocException);
        }
    }

    void Server::stop() {
        m_listener->stop();
        m_ioc.stop();
    }
}

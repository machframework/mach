#include "Server.hpp"

#include <exception>
#include <format>
#include <mutex>
#include <thread>
#include <vector>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>

#include <mach/Logger.hpp>

namespace mach::detail::server
{
    Server::Server(
        AppOptions&& appOptions,
        di::Container container,
        middleware::MiddlewarePipeline middlewarePipeline,
        const mach::Logger& logger)
        : m_appOptions(std::move(appOptions)),
          m_endpoint(boost::asio::ip::make_address(m_appOptions.host), m_appOptions.port),
          m_ioc(static_cast<int>(m_appOptions.threadCount)), m_runtime(m_appOptions, std::move(container), std::move(middlewarePipeline), logger),
          m_logger(logger) {}

    std::string Server::host() const noexcept {
        return m_appOptions.host;
    }

    std::uint16_t Server::port() const noexcept {
        return m_appOptions.port;
    }

    std::size_t Server::threadCount() const noexcept {
        return m_appOptions.threadCount;
    }

    void Server::run() {
        m_listener = std::make_shared<BeastListener>(
            m_appOptions,
            m_ioc,
            m_endpoint,
            m_runtime,
            m_requestAdapter,
            m_responseAdapter,
            m_logger);

        // configure signals
        net::signal_set signals(m_ioc, SIGINT, SIGTERM);
        signals.async_wait([this](boost::system::error_code ec, int) {
            if (!ec) {
                stop();
            }
        });

        net::co_spawn(m_ioc, m_listener->run(), net::detached);

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> threads;
        threads.reserve(m_appOptions.threadCount - 1);

        m_logger
            .info("Starting Mach server on {}:{} with {} threads", host(), port(), threadCount());

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

        for (int i = 0; i < m_appOptions.threadCount - 1; ++i) {
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

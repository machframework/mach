#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>

#include <boost/asio.hpp>

#include "Testing.hpp"

#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

using namespace std::chrono_literals;

namespace
{
    constexpr std::string_view host = "127.0.0.1";

    constexpr auto startupTimeout = 5s;
    constexpr auto shutdownTimeout = 5s;
    constexpr auto retryDelay = 10ms;
    constexpr auto unexpectedRunTimeout = 500ms;

    auto makeApp(std::uint16_t port)
    {
        auto& options = testing::serverOptions;

        options.host = std::string{ host };
        options.port = port;

        return mach::AppBuilder(std::move(options)).build();
    }

    [[noreturn]]
    void fail(const std::string& message)
    {
        throw std::runtime_error(message);
    }

    void require(
        bool condition,
        const std::string& message
    )
    {
        if (!condition) {
            fail(message);
        }
    }

    bool canConnect(std::uint16_t port)
    {
        namespace net = boost::asio;
        using tcp = net::ip::tcp;

        net::io_context ioContext;
        tcp::socket socket{ ioContext };

        boost::system::error_code ec;

        const auto address =
            net::ip::make_address(std::string{ host }, ec);

        if (ec) {
            return false;
        }

        socket.connect(
            tcp::endpoint{ address, port },
            ec
        );

        if (ec) {
            return false;
        }

        socket.close(ec);

        return true;
    }

    void waitUntilListening(std::uint16_t port)
    {
        const auto deadline =
            std::chrono::steady_clock::now() +
            startupTimeout;

        while (
            std::chrono::steady_clock::now() <
            deadline
            ) {
            if (canConnect(port)) {
                return;
            }

            std::this_thread::sleep_for(retryDelay);
        }

        fail(
            "The server did not begin listening before "
            "the startup timeout."
        );
    }

    void waitUntilNotListening(std::uint16_t port)
    {
        const auto deadline =
            std::chrono::steady_clock::now() +
            shutdownTimeout;

        while (
            std::chrono::steady_clock::now() <
            deadline
            ) {
            if (!canConnect(port)) {
                return;
            }

            std::this_thread::sleep_for(retryDelay);
        }

        fail(
            "The server remained reachable after shutdown."
        );
    }

    struct RunThread
    {
        std::thread thread;

        std::atomic<bool> completed{ false };
        std::atomic<int> result{ -1 };

        std::mutex exceptionMutex;
        std::exception_ptr exception;
    };

    void startAppOnThread(
        mach::App& app,
        RunThread& runThread
    )
    {
        runThread.thread = std::thread([&] {
            try {
                runThread.result.store(
                    app.run(),
                    std::memory_order_release
                );
            }
            catch (...) {
                std::scoped_lock lock{
                    runThread.exceptionMutex
                };

                runThread.exception =
                    std::current_exception();
            }

            runThread.completed.store(
                true,
                std::memory_order_release
            );
            });
    }

    std::exception_ptr getException(
        RunThread& runThread
    )
    {
        std::scoped_lock lock{
            runThread.exceptionMutex
        };

        return runThread.exception;
    }

    void joinRunThread(RunThread& runThread)
    {
        if (runThread.thread.joinable()) {
            runThread.thread.join();
        }
    }

    void joinAndRethrow(RunThread& runThread)
    {
        joinRunThread(runThread);

        if (const auto exception =
            getException(runThread)) {
            std::rethrow_exception(exception);
        }
    }

    void requireLogicError(
        const std::function<void()>& operation,
        const std::string& failureMessage
    )
    {
        try {
            operation();
        }
        catch (const std::logic_error&) {
            return;
        }
        catch (const std::exception& exception) {
            fail(
                failureMessage +
                " Wrong std::exception type: " +
                exception.what()
            );
        }
        catch (...) {
            fail(
                failureMessage +
                " A non-standard exception was thrown."
            );
        }

        fail(
            failureMessage +
            " No exception was thrown."
        );
    }

    template <typename Test>
    void runTest(
        std::string_view name,
        Test&& test
    )
    {
        std::cout
            << "[TEST] "
            << name
            << '\n';

        std::forward<Test>(test)();

        std::cout
            << "[PASS] "
            << name
            << "\n\n";
    }

    // ---------------------------------------------------------------------
    // stop() before run()
    // ---------------------------------------------------------------------

    void testStopBeforeRunIsHarmless()
    {
        constexpr std::uint16_t port = 3143;

        auto app = makeApp(port);

        app.stop();
        app.stop();
    }

    void testStopBeforeRunDoesNotConsumeApplication()
    {
        constexpr std::uint16_t port = 3144;

        auto app = makeApp(port);

        app.stop();

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();

        joinAndRethrow(runThread);

        require(
            runThread.result.load(
                std::memory_order_acquire
            ) == 0,
            "run() did not return 0 after normal shutdown."
        );

        waitUntilNotListening(port);
    }

    // ---------------------------------------------------------------------
    // Normal shutdown
    // ---------------------------------------------------------------------

    void testStopTerminatesRunningApplication()
    {
        constexpr std::uint16_t port = 3145;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();

        joinAndRethrow(runThread);

        require(
            runThread.result.load(
                std::memory_order_acquire
            ) == 0,
            "run() did not return 0 after stop()."
        );

        waitUntilNotListening(port);
    }

    void testRepeatedStopIsHarmless()
    {
        constexpr std::uint16_t port = 3146;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();
        app.stop();
        app.stop();

        joinAndRethrow(runThread);

        require(
            runThread.result.load(
                std::memory_order_acquire
            ) == 0,
            "Repeated stop() calls changed the "
            "normal shutdown result."
        );

        waitUntilNotListening(port);
    }

    void testStopAfterShutdownIsHarmless()
    {
        constexpr std::uint16_t port = 3147;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();

        joinAndRethrow(runThread);

        app.stop();
        app.stop();

        waitUntilNotListening(port);
    }

    void testConcurrentStopCallsAreSafe()
    {
        constexpr std::uint16_t port = 3148;
        constexpr std::size_t stopThreadCount = 16;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        std::promise<void> releasePromise;

        std::shared_future<void> releaseSignal =
            releasePromise.get_future().share();

        std::atomic<std::size_t> readyCount{ 0 };
        std::atomic<std::size_t> completedCount{ 0 };

        std::vector<std::thread> stopThreads;
        stopThreads.reserve(stopThreadCount);

        for (
            std::size_t index = 0;
            index < stopThreadCount;
            ++index
            ) {
            stopThreads.emplace_back([&] {
                readyCount.fetch_add(
                    1,
                    std::memory_order_acq_rel
                );

                releaseSignal.wait();

                app.stop();

                completedCount.fetch_add(
                    1,
                    std::memory_order_acq_rel
                );
                });
        }

        while (
            readyCount.load(
                std::memory_order_acquire
            ) < stopThreadCount
            ) {
            std::this_thread::yield();
        }

        releasePromise.set_value();

        for (auto& thread : stopThreads) {
            thread.join();
        }

        joinAndRethrow(runThread);

        require(
            completedCount.load(
                std::memory_order_acquire
            ) == stopThreadCount,
            "At least one concurrent stop() call "
            "did not complete."
        );

        require(
            runThread.result.load(
                std::memory_order_acquire
            ) == 0,
            "Concurrent stop() calls caused run() to fail."
        );

        waitUntilNotListening(port);
    }

    // ---------------------------------------------------------------------
    // Duplicate run()
    // ---------------------------------------------------------------------

    void testSecondRunWhileRunningThrowsLogicError()
    {
        constexpr std::uint16_t port = 3149;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        requireLogicError(
            [&] {
                app.run();
            },
            "A second concurrent run() call was not "
            "rejected correctly."
        );

        require(
            canConnect(port),
            "The rejected run() call stopped the "
            "original server."
        );

        app.stop();

        joinAndRethrow(runThread);

        require(
            runThread.result.load(
                std::memory_order_acquire
            ) == 0,
            "The original run() failed after rejecting "
            "a duplicate run()."
        );

        waitUntilNotListening(port);
    }

    void testRepeatedDuplicateRunCallsAllThrow()
    {
        constexpr std::uint16_t port = 3150;
        constexpr int attemptCount = 10;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        for (
            int attempt = 0;
            attempt < attemptCount;
            ++attempt
            ) {
            requireLogicError(
                [&] {
                    app.run();
                },
                "A duplicate run() attempt was not rejected."
            );

            require(
                canConnect(port),
                "A duplicate run() attempt affected "
                "the active server."
            );
        }

        app.stop();

        joinAndRethrow(runThread);

        require(
            runThread.result.load(
                std::memory_order_acquire
            ) == 0,
            "The original server failed after repeated "
            "duplicate run() attempts."
        );

        waitUntilNotListening(port);
    }

    void testManyConcurrentRunCallsAllowExactlyOneRunner()
    {
        constexpr std::uint16_t port = 3151;
        constexpr std::size_t callerCount = 16;

        auto app = makeApp(port);

        std::promise<void> releasePromise;

        std::shared_future<void> releaseSignal =
            releasePromise.get_future().share();

        std::atomic<std::size_t> readyCount{ 0 };
        std::atomic<std::size_t> successfulRuns{ 0 };
        std::atomic<std::size_t> logicErrors{ 0 };
        std::atomic<std::size_t> wrongResults{ 0 };

        std::vector<std::thread> callers;
        callers.reserve(callerCount);

        for (
            std::size_t index = 0;
            index < callerCount;
            ++index
            ) {
            callers.emplace_back([&] {
                readyCount.fetch_add(
                    1,
                    std::memory_order_acq_rel
                );

                releaseSignal.wait();

                try {
                    const int result = app.run();

                    if (result == 0) {
                        successfulRuns.fetch_add(
                            1,
                            std::memory_order_acq_rel
                        );
                    }
                    else {
                        wrongResults.fetch_add(
                            1,
                            std::memory_order_acq_rel
                        );
                    }
                }
                catch (const std::logic_error&) {
                    logicErrors.fetch_add(
                        1,
                        std::memory_order_acq_rel
                    );
                }
                catch (...) {
                    wrongResults.fetch_add(
                        1,
                        std::memory_order_acq_rel
                    );
                }
                });
        }

        while (
            readyCount.load(
                std::memory_order_acquire
            ) < callerCount
            ) {
            std::this_thread::yield();
        }

        releasePromise.set_value();

        waitUntilListening(port);

        require(
            canConnect(port),
            "No concurrent run() caller successfully "
            "started the server."
        );

        app.stop();

        for (auto& caller : callers) {
            caller.join();
        }

        require(
            successfulRuns.load(
                std::memory_order_acquire
            ) == 1,
            "Concurrent run() calls did not produce "
            "exactly one successful runner."
        );

        require(
            logicErrors.load(
                std::memory_order_acquire
            ) == callerCount - 1,
            "Not every losing run() caller received "
            "std::logic_error."
        );

        require(
            wrongResults.load(
                std::memory_order_acquire
            ) == 0,
            "A concurrent run() caller returned an "
            "unexpected result or threw the wrong exception."
        );

        waitUntilNotListening(port);
    }

    // ---------------------------------------------------------------------
    // Single-use lifecycle
    // ---------------------------------------------------------------------

    void testRunAfterNormalShutdownThrows()
    {
        constexpr std::uint16_t port = 3152;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();

        joinAndRethrow(runThread);

        requireLogicError(
            [&] {
                app.run();
            },
            "run() after shutdown did not reject "
            "application reuse."
        );

        waitUntilNotListening(port);
    }

    void testRunAfterShutdownAlwaysThrows()
    {
        constexpr std::uint16_t port = 3153;
        constexpr int attemptCount = 10;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();

        joinAndRethrow(runThread);

        for (
            int attempt = 0;
            attempt < attemptCount;
            ++attempt
            ) {
            requireLogicError(
                [&] {
                    app.run();
                },
                "A completed App instance was reusable."
            );
        }

        waitUntilNotListening(port);
    }

    void testStopAfterRejectedRestartIsHarmless()
    {
        constexpr std::uint16_t port = 3154;

        auto app = makeApp(port);

        RunThread runThread;
        startAppOnThread(app, runThread);

        waitUntilListening(port);

        app.stop();

        joinAndRethrow(runThread);

        requireLogicError(
            [&] {
                app.run();
            },
            "Restart was not rejected."
        );

        app.stop();
        app.stop();

        waitUntilNotListening(port);
    }

    // ---------------------------------------------------------------------
    // Bind failure
    // ---------------------------------------------------------------------

    void testBindFailureReturnsOneInsteadOfThrowing()
    {
        constexpr std::uint16_t port = 3155;

        auto firstApp = makeApp(port);
        auto secondApp = makeApp(port);

        RunThread firstRun;
        startAppOnThread(firstApp, firstRun);

        waitUntilListening(port);

        RunThread secondRun;
        startAppOnThread(secondApp, secondRun);

        const auto deadline =
            std::chrono::steady_clock::now() +
            unexpectedRunTimeout;

        while (
            !secondRun.completed.load(
                std::memory_order_acquire
            ) &&
            std::chrono::steady_clock::now() <
            deadline
            ) {
            std::this_thread::sleep_for(retryDelay);
        }

        const bool secondRunReturned =
            secondRun.completed.load(
                std::memory_order_acquire
            );

        if (!secondRunReturned) {
            secondApp.stop();
        }

        firstApp.stop();

        joinRunThread(secondRun);
        joinAndRethrow(firstRun);

        if (!secondRunReturned) {
            fail(
                "The second application entered its server loop "
                "instead of failing to bind. The platform or socket "
                "configuration allowed two listeners to bind the "
                "same endpoint."
            );
        }

        if (const auto exception =
            getException(secondRun)) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const std::exception& error) {
                fail(
                    "The bind failure escaped run(): " +
                    std::string{ error.what() }
                );
            }
            catch (...) {
                fail(
                    "The bind failure escaped run() as a "
                    "non-standard exception."
                );
            }
        }

        require(
            secondRun.result.load(
                std::memory_order_acquire
            ) == 1,
            "A fatal bind failure did not make run() return 1."
        );

        waitUntilNotListening(port);
    }

    void testFailedRunConsumesApplication()
    {
        constexpr std::uint16_t port = 3156;

        auto firstApp = makeApp(port);
        auto secondApp = makeApp(port);

        RunThread firstRun;
        startAppOnThread(firstApp, firstRun);

        waitUntilListening(port);

        RunThread secondRun;
        startAppOnThread(secondApp, secondRun);

        const auto deadline =
            std::chrono::steady_clock::now() +
            unexpectedRunTimeout;

        while (
            !secondRun.completed.load(
                std::memory_order_acquire
            ) &&
            std::chrono::steady_clock::now() <
            deadline
            ) {
            std::this_thread::sleep_for(retryDelay);
        }

        const bool secondRunReturned =
            secondRun.completed.load(
                std::memory_order_acquire
            );

        if (!secondRunReturned) {
            secondApp.stop();
        }

        firstApp.stop();

        joinRunThread(secondRun);
        joinAndRethrow(firstRun);

        if (!secondRunReturned) {
            fail(
                "The bind-failure precondition was not created. "
                "The second application successfully entered its "
                "server loop on the occupied endpoint."
            );
        }

        if (const auto exception =
            getException(secondRun)) {
            std::rethrow_exception(exception);
        }

        require(
            secondRun.result.load(
                std::memory_order_acquire
            ) == 1,
            "The intentionally failed run() did not return 1."
        );

        requireLogicError(
            [&] {
                secondApp.run();
            },
            "An App was reusable after a failed run() attempt."
        );

        secondApp.stop();

        waitUntilNotListening(port);
    }
}

int main()
{
    try {
        runTest(
            "stop before run is harmless",
            testStopBeforeRunIsHarmless
        );

        runTest(
            "stop before run does not consume application",
            testStopBeforeRunDoesNotConsumeApplication
        );

        runTest(
            "stop terminates running application",
            testStopTerminatesRunningApplication
        );

        runTest(
            "repeated stop is harmless",
            testRepeatedStopIsHarmless
        );

        runTest(
            "stop after shutdown is harmless",
            testStopAfterShutdownIsHarmless
        );

        runTest(
            "concurrent stop calls are safe",
            testConcurrentStopCallsAreSafe
        );

        runTest(
            "second run while running throws logic_error",
            testSecondRunWhileRunningThrowsLogicError
        );

        runTest(
            "repeated duplicate run calls all throw",
            testRepeatedDuplicateRunCallsAllThrow
        );

        runTest(
            "many concurrent run calls allow exactly one runner",
            testManyConcurrentRunCallsAllowExactlyOneRunner
        );

        runTest(
            "run after normal shutdown throws",
            testRunAfterNormalShutdownThrows
        );

        runTest(
            "run after shutdown always throws",
            testRunAfterShutdownAlwaysThrows
        );

        runTest(
            "stop after rejected restart is harmless",
            testStopAfterRejectedRestartIsHarmless
        );

        runTest(
            "bind failure returns one instead of throwing",
            testBindFailureReturnsOneInsteadOfThrowing
        );

        runTest(
            "failed run consumes application",
            testFailedRunConsumesApplication
        );
    }
    catch (const std::exception& exception) {
        std::cerr
            << "\nApp lifecycle test failure: "
            << exception.what()
            << '\n';

        return 1;
    }
    catch (...) {
        std::cerr
            << "\nApp lifecycle test failure: "
            << "unknown exception\n";

        return 1;
    }

    std::cout
        << "All App lifecycle firewall tests passed.\n";

    return 0;
}

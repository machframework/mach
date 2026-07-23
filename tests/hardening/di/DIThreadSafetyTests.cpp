#include <atomic>
#include <barrier>
#include <cstddef>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;

    class ConcurrentSingleton {
    public:
        ConcurrentSingleton() {
            ++s_constructionCount;

            // Increase the chance that several threads reach
            // singleton resolution while construction is in progress.
            std::this_thread::yield();
        }

        static void reset() noexcept {
            s_constructionCount = 0;
        }

        [[nodiscard]]
        static int constructionCount() noexcept {
            return s_constructionCount.load();
        }

    private:
        inline static std::atomic<int> s_constructionCount = 0;
    };

    class ConcurrentScopedService {
    public:
        ConcurrentScopedService() : m_id(++s_nextId) {
            ++s_constructionCount;
        }

        static void reset() noexcept {
            s_nextId = 0;
            s_constructionCount = 0;
        }

        [[nodiscard]]
        int id() const noexcept {
            return m_id;
        }

        [[nodiscard]]
        static int constructionCount() noexcept {
            return s_constructionCount.load();
        }

    private:
        int m_id;

        inline static std::atomic<int> s_nextId = 0;
        inline static std::atomic<int> s_constructionCount = 0;
    };

    class FirstIndependentSingleton {
    public:
        FirstIndependentSingleton() {
            s_constructorEntered = true;

            while (!s_otherConstructorEntered->load()) {
                std::this_thread::yield();
            }
        }

        static void configure(std::atomic<bool>& otherConstructorEntered) noexcept {
            s_constructorEntered = false;
            s_otherConstructorEntered = &otherConstructorEntered;
        }

        [[nodiscard]]
        static std::atomic<bool>& constructorEntered() noexcept {
            return s_constructorEntered;
        }

    private:
        inline static std::atomic<bool> s_constructorEntered = false;

        inline static std::atomic<bool>* s_otherConstructorEntered = nullptr;
    };

    class SecondIndependentSingleton {
    public:
        SecondIndependentSingleton() {
            s_constructorEntered = true;

            while (!s_otherConstructorEntered->load()) {
                std::this_thread::yield();
            }
        }

        static void configure(std::atomic<bool>& otherConstructorEntered) noexcept {
            s_constructorEntered = false;
            s_otherConstructorEntered = &otherConstructorEntered;
        }

        [[nodiscard]]
        static std::atomic<bool>& constructorEntered() noexcept {
            return s_constructorEntered;
        }

    private:
        inline static std::atomic<bool> s_constructorEntered = false;

        inline static std::atomic<bool>* s_otherConstructorEntered = nullptr;
    };

    class FailingConcurrentSingleton {
    public:
        FailingConcurrentSingleton() {
            const int attempt = ++s_constructionAttempts;

            if (attempt == 1) {
                throw std::runtime_error("Intentional singleton construction failure");
            }

            ++s_successfulConstructions;
        }

        static void reset() noexcept {
            s_constructionAttempts = 0;
            s_successfulConstructions = 0;
        }

        [[nodiscard]]
        static int constructionAttempts() noexcept {
            return s_constructionAttempts.load();
        }

        [[nodiscard]]
        static int successfulConstructions() noexcept {
            return s_successfulConstructions.load();
        }

    private:
        inline static std::atomic<int> s_constructionAttempts = 0;
        inline static std::atomic<int> s_successfulConstructions = 0;
    };

    void testConcurrentSingletonConstructedOnce() {
        constexpr std::string_view testName = "Concurrent singleton is constructed exactly once";

        ConcurrentSingleton::reset();

        di::Container container;

        container.addService<ConcurrentSingleton>(di::ServiceLifetime::Singleton);

        container.finalizeRegistrations();

        constexpr std::size_t threadCount = 32;

        std::barrier startBarrier(static_cast<std::ptrdiff_t>(threadCount));

        std::vector<const ConcurrentSingleton*> instances(threadCount, nullptr);

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        std::mutex exceptionMutex;
        std::exception_ptr threadException;

        for (std::size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([&, i] {
                try {
                    auto scope = container.createScope();

                    startBarrier.arrive_and_wait();

                    instances[i] = &scope.resolve<ConcurrentSingleton>();
                } catch (...) {
                    std::lock_guard lock(exceptionMutex);

                    if (!threadException) {
                        threadException = std::current_exception();
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        if (threadException) {
            try {
                std::rethrow_exception(threadException);
            } catch (const std::exception& exception) {
                testing::fail(testName, exception.what());
            }

            return;
        }

        if (ConcurrentSingleton::constructionCount() != 1) {
            testing::fail(testName, "Singleton constructor ran more than once");

            return;
        }

        const auto* expectedInstance = instances.front();

        if (expectedInstance == nullptr) {
            testing::fail(testName, "Singleton resolution returned a null address");

            return;
        }

        for (const auto* instance : instances) {
            if (instance != expectedInstance) {
                testing::fail(
                    testName,
                    "Concurrent resolutions returned different singleton instances");

                return;
            }
        }

        testing::success(testName);
    }

    void testSeparateScopesResolveConcurrently() {
        constexpr std::string_view testName =
            "Separate scopes resolve scoped services concurrently";

        ConcurrentScopedService::reset();

        di::Container container;

        container.addService<ConcurrentScopedService>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        constexpr std::size_t threadCount = 32;

        std::barrier startBarrier(static_cast<std::ptrdiff_t>(threadCount));

        std::vector<int> instanceIds(threadCount, 0);

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        std::mutex exceptionMutex;
        std::exception_ptr threadException;

        for (std::size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([&, i] {
                try {
                    auto scope = container.createScope();

                    startBarrier.arrive_and_wait();

                    const auto& instance = scope.resolve<ConcurrentScopedService>();

                    instanceIds[i] = instance.id();
                } catch (...) {
                    std::lock_guard lock(exceptionMutex);

                    if (!threadException) {
                        threadException = std::current_exception();
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        if (threadException) {
            try {
                std::rethrow_exception(threadException);
            } catch (const std::exception& exception) {
                testing::fail(testName, exception.what());
            }

            return;
        }

        if (ConcurrentScopedService::constructionCount() != static_cast<int>(threadCount)) {
            testing::fail(testName, "Scoped service was not constructed once per scope");

            return;
        }

        for (std::size_t i = 0; i < instanceIds.size(); ++i) {
            if (instanceIds[i] == 0) {
                testing::fail(testName, "A scoped service was not resolved");

                return;
            }

            for (std::size_t j = i + 1; j < instanceIds.size(); ++j) {
                if (instanceIds[i] == instanceIds[j]) {
                    testing::fail(
                        testName,
                        "Different scopes received the same scoped instance ID");

                    return;
                }
            }
        }

        testing::success(testName);
    }

    void testIndependentSingletonsInitializeConcurrently() {
        constexpr std::string_view testName = "Independent singletons initialize concurrently";

        FirstIndependentSingleton::configure(SecondIndependentSingleton::constructorEntered());

        SecondIndependentSingleton::configure(FirstIndependentSingleton::constructorEntered());

        di::Container container;

        container.addService<FirstIndependentSingleton>(di::ServiceLifetime::Singleton);

        container.addService<SecondIndependentSingleton>(di::ServiceLifetime::Singleton);

        container.finalizeRegistrations();

        std::barrier startBarrier(2);

        std::exception_ptr firstException;
        std::exception_ptr secondException;

        std::thread firstThread([&] {
            try {
                auto scope = container.createScope();

                startBarrier.arrive_and_wait();

                [[maybe_unused]]
                auto& instance = scope.resolve<FirstIndependentSingleton>();
            } catch (...) {
                firstException = std::current_exception();
            }
        });

        std::thread secondThread([&] {
            try {
                auto scope = container.createScope();

                startBarrier.arrive_and_wait();

                [[maybe_unused]]
                auto& instance = scope.resolve<SecondIndependentSingleton>();
            } catch (...) {
                secondException = std::current_exception();
            }
        });

        firstThread.join();
        secondThread.join();

        if (firstException) {
            try {
                std::rethrow_exception(firstException);
            } catch (const std::exception& exception) {
                testing::fail(testName, exception.what());
            }

            return;
        }

        if (secondException) {
            try {
                std::rethrow_exception(secondException);
            } catch (const std::exception& exception) {
                testing::fail(testName, exception.what());
            }

            return;
        }

        if (!FirstIndependentSingleton::constructorEntered().load() ||
            !SecondIndependentSingleton::constructorEntered().load()) {
            testing::fail(testName, "Both singleton constructors did not begin");

            return;
        }

        testing::success(testName);
    }

    void testConcurrentSingletonFailureCanRetry() {
        constexpr std::string_view testName = "Concurrent singleton construction failure can retry";

        FailingConcurrentSingleton::reset();

        di::Container container;

        container.addService<FailingConcurrentSingleton>(di::ServiceLifetime::Singleton);

        container.finalizeRegistrations();

        constexpr std::size_t threadCount = 16;

        std::barrier startBarrier(static_cast<std::ptrdiff_t>(threadCount));

        std::vector<const FailingConcurrentSingleton*> instances(threadCount, nullptr);

        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        std::atomic<int> failureCount = 0;

        std::mutex exceptionMutex;
        std::exception_ptr unexpectedException;

        for (std::size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([&, i] {
                try {
                    auto scope = container.createScope();

                    startBarrier.arrive_and_wait();

                    instances[i] = &scope.resolve<FailingConcurrentSingleton>();
                } catch (const std::runtime_error&) {
                    ++failureCount;
                } catch (...) {
                    std::lock_guard lock(exceptionMutex);

                    if (!unexpectedException) {
                        unexpectedException = std::current_exception();
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        if (unexpectedException) {
            try {
                std::rethrow_exception(unexpectedException);
            } catch (const std::exception& exception) {
                testing::fail(testName, exception.what());
            }

            return;
        }

        if (failureCount.load() != 1) {
            testing::fail(testName, "Expected exactly one failed initialization attempt");

            return;
        }

        if (FailingConcurrentSingleton::constructionAttempts() != 2) {
            testing::fail(
                testName,
                "Singleton construction was attempted an unexpected number of times");

            return;
        }

        if (FailingConcurrentSingleton::successfulConstructions() != 1) {
            testing::fail(testName, "Singleton was successfully constructed more than once");

            return;
        }

        const FailingConcurrentSingleton* successfulInstance = nullptr;

        for (const auto* instance : instances) {
            if (instance == nullptr) {
                continue;
            }

            if (successfulInstance == nullptr) {
                successfulInstance = instance;
                continue;
            }

            if (instance != successfulInstance) {
                testing::fail(
                    testName,
                    "Successful threads received different singleton instances");

                return;
            }
        }

        if (successfulInstance == nullptr) {
            testing::fail(testName, "No thread successfully resolved the singleton");

            return;
        }

        testing::success(testName);
    }
}

int main() {
    testConcurrentSingletonConstructedOnce();
    testSeparateScopesResolveConcurrently();
    testIndependentSingletonsInitializeConcurrently();
    testConcurrentSingletonFailureCanRetry();

    return 0;
}

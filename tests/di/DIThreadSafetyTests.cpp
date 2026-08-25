#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <barrier>
#include <cstddef>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

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

    void rethrowIfSet(const std::exception_ptr& exception) {
        if (exception) {
            std::rethrow_exception(exception);
        }
    }
}

TEST_CASE("DI resolves singleton safely under concurrency") {
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

    REQUIRE_NOTHROW(rethrowIfSet(threadException));

    REQUIRE(ConcurrentSingleton::constructionCount() == 1);

    const auto* expectedInstance = instances.front();

    REQUIRE(expectedInstance != nullptr);

    for (const auto* instance : instances) {
        REQUIRE(instance == expectedInstance);
    }
}

TEST_CASE("DI resolves scoped services independently across threads") {
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

                instanceIds[i] = scope.resolve<ConcurrentScopedService>().id();
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

    REQUIRE_NOTHROW(rethrowIfSet(threadException));

    REQUIRE(ConcurrentScopedService::constructionCount() == static_cast<int>(threadCount));

    for (std::size_t i = 0; i < instanceIds.size(); ++i) {
        REQUIRE(instanceIds[i] != 0);

        for (std::size_t j = i + 1; j < instanceIds.size(); ++j) {
            REQUIRE(instanceIds[i] != instanceIds[j]);
        }
    }
}

TEST_CASE("DI initializes independent singletons concurrently") {
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

            scope.resolve<FirstIndependentSingleton>();
        } catch (...) {
            firstException = std::current_exception();
        }
    });

    std::thread secondThread([&] {
        try {
            auto scope = container.createScope();

            startBarrier.arrive_and_wait();

            scope.resolve<SecondIndependentSingleton>();
        } catch (...) {
            secondException = std::current_exception();
        }
    });

    firstThread.join();
    secondThread.join();

    REQUIRE_NOTHROW(rethrowIfSet(firstException));
    REQUIRE_NOTHROW(rethrowIfSet(secondException));

    REQUIRE(FirstIndependentSingleton::constructorEntered().load());

    REQUIRE(SecondIndependentSingleton::constructorEntered().load());
}

TEST_CASE("DI retries failed singleton construction safely under concurrency") {
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

    REQUIRE_NOTHROW(rethrowIfSet(unexpectedException));

    REQUIRE(failureCount.load() == 1);
    REQUIRE(FailingConcurrentSingleton::constructionAttempts() == 2);
    REQUIRE(FailingConcurrentSingleton::successfulConstructions() == 1);

    const FailingConcurrentSingleton* successfulInstance = nullptr;

    for (const auto* instance : instances) {
        if (instance == nullptr) {
            continue;
        }

        if (successfulInstance == nullptr) {
            successfulInstance = instance;
        } else {
            REQUIRE(instance == successfulInstance);
        }
    }

    REQUIRE(successfulInstance != nullptr);
}

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string_view>

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>
#include <mach/detail/routing/Router.hpp>

namespace
{
    namespace di = mach::detail::di;

    // -------------------------------------------------------------------------
    // Basic resolution
    // -------------------------------------------------------------------------

    struct Logger {
        int value = 10;
    };

    struct Repository {
        explicit Repository(Logger& logger) : logger(logger) {}

        Logger& logger;
    };

    struct Service {
        Service(Repository& repository, Logger& logger) : repository(repository), logger(logger) {}

        Repository& repository;
        Logger& logger;
    };

    struct Controller {
        explicit Controller(Service& service) : service(service) {}

        [[nodiscard]]
        int handle() const {
            return service.repository.logger.value + service.logger.value;
        }

        Service& service;
    };

    struct EmptyService {};

    // -------------------------------------------------------------------------
    // Lifetimes and dependency identity
    // -------------------------------------------------------------------------

    struct Counter {
        int value = 0;
    };

    struct Dependency {};

    struct FirstConsumer {
        explicit FirstConsumer(Dependency& dependency) : dependency(dependency) {}

        Dependency& dependency;
    };

    struct SecondConsumer {
        explicit SecondConsumer(Dependency& dependency) : dependency(dependency) {}

        Dependency& dependency;
    };

    // -------------------------------------------------------------------------
    // Resolution validation
    // -------------------------------------------------------------------------

    struct MissingDependency {};

    struct RequiresMissingDependency {
        explicit RequiresMissingDependency(MissingDependency& dependency)
            : dependency(dependency) {}

        MissingDependency& dependency;
    };

    struct FirstCircularService;

    struct SecondCircularService {
        explicit SecondCircularService(FirstCircularService& service) : service(service) {}

        FirstCircularService& service;
    };

    struct FirstCircularService {
        explicit FirstCircularService(SecondCircularService& service) : service(service) {}

        SecondCircularService& service;
    };

    // -------------------------------------------------------------------------
    // Lifetime validation
    // -------------------------------------------------------------------------

    struct ScopedDependency {};

    struct TransientScopedConsumer {
        explicit TransientScopedConsumer(ScopedDependency& dependency) : dependency(dependency) {}

        ScopedDependency& dependency;
    };

    struct MiddleTransientConsumer {
        explicit MiddleTransientConsumer(TransientScopedConsumer& consumer) : consumer(consumer) {}

        TransientScopedConsumer& consumer;
    };

    struct DirectScopedSingleton {
        explicit DirectScopedSingleton(ScopedDependency& dependency) : dependency(dependency) {}

        ScopedDependency& dependency;
    };

    struct IndirectScopedSingleton {
        explicit IndirectScopedSingleton(TransientScopedConsumer& consumer) : consumer(consumer) {}

        TransientScopedConsumer& consumer;
    };

    struct DeepScopedSingleton {
        explicit DeepScopedSingleton(MiddleTransientConsumer& consumer) : consumer(consumer) {}

        MiddleTransientConsumer& consumer;
    };

    struct SingletonDependency {};

    struct SingletonConsumer {
        explicit SingletonConsumer(SingletonDependency& dependency) : dependency(dependency) {}

        SingletonDependency& dependency;
    };

    struct TransientDependency {};

    struct SingletonWithTransient {
        explicit SingletonWithTransient(TransientDependency& dependency) : dependency(dependency) {}

        TransientDependency& dependency;
    };

    // -------------------------------------------------------------------------
    // Construction exception safety
    // -------------------------------------------------------------------------

    class ThrowingService {
    public:
        ThrowingService() {
            ++s_constructionAttempts;

            if (s_shouldThrow) {
                throw std::runtime_error("Intentional construction failure");
            }
        }

        static void reset() noexcept {
            s_constructionAttempts = 0;
            s_shouldThrow = true;
        }

        static void allowConstruction() noexcept {
            s_shouldThrow = false;
        }

        [[nodiscard]]
        static int constructionAttempts() noexcept {
            return s_constructionAttempts;
        }

    private:
        inline static int s_constructionAttempts = 0;
        inline static bool s_shouldThrow = true;
    };

    class CachedDependency {
    public:
        CachedDependency() {
            ++s_constructions;
        }

        static void reset() noexcept {
            s_constructions = 0;
        }

        [[nodiscard]]
        static int constructions() noexcept {
            return s_constructions;
        }

    private:
        inline static int s_constructions = 0;
    };

    class ThrowingConsumer {
    public:
        explicit ThrowingConsumer(CachedDependency& dependency) : dependency(dependency) {
            ++s_constructionAttempts;

            if (s_shouldThrow) {
                throw std::runtime_error("Intentional consumer construction failure");
            }
        }

        static void reset() noexcept {
            s_constructionAttempts = 0;
            s_shouldThrow = true;
        }

        static void allowConstruction() noexcept {
            s_shouldThrow = false;
        }

        [[nodiscard]]
        static int constructionAttempts() noexcept {
            return s_constructionAttempts;
        }

        CachedDependency& dependency;

    private:
        inline static int s_constructionAttempts = 0;
        inline static bool s_shouldThrow = true;
    };

    // -------------------------------------------------------------------------
    // Resource lifetime
    // -------------------------------------------------------------------------

    class TrackedScopedService {
    public:
        TrackedScopedService() {
            ++s_aliveCount;
        }

        ~TrackedScopedService() {
            --s_aliveCount;
        }

        static void reset() noexcept {
            s_aliveCount = 0;
        }

        [[nodiscard]]
        static int aliveCount() noexcept {
            return s_aliveCount;
        }

    private:
        inline static int s_aliveCount = 0;
    };

    class TrackedSingleton {
    public:
        TrackedSingleton() {
            ++s_aliveCount;
        }

        ~TrackedSingleton() {
            --s_aliveCount;
        }

        static void reset() noexcept {
            s_aliveCount = 0;
        }

        [[nodiscard]]
        static int aliveCount() noexcept {
            return s_aliveCount;
        }

    private:
        inline static int s_aliveCount = 0;
    };

    // -------------------------------------------------------------------------
    // Immovable services
    // -------------------------------------------------------------------------

    class ImmovableService {
    public:
        ImmovableService() = default;

        ImmovableService(const ImmovableService&) = delete;
        ImmovableService& operator=(const ImmovableService&) = delete;

        ImmovableService(ImmovableService&&) = delete;
        ImmovableService& operator=(ImmovableService&&) = delete;
    };

    struct ImmovableConsumer {
        explicit ImmovableConsumer(ImmovableService& service) : service(service) {}

        ImmovableService& service;
    };

    // -------------------------------------------------------------------------
    // Reserved services
    // -------------------------------------------------------------------------

    using mach::detail::binding::BodyBinder;
    using mach::detail::routing::Router;
}

TEST_CASE("DI resolves registered services") {
    SECTION("Dependency graph") {
        di::Container container;

        container.addService<Logger>(di::ServiceLifetime::Scoped);
        container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
        container.addService<Service, Repository, Logger>(di::ServiceLifetime::Scoped);
        container.addService<Controller, Service>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE(scope.resolve<Controller>().handle() == 20);
    }

    SECTION("Registration order does not matter") {
        di::Container container;

        container.addService<Controller, Service>(di::ServiceLifetime::Scoped);
        container.addService<Service, Repository, Logger>(di::ServiceLifetime::Scoped);
        container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
        container.addService<Logger>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE(scope.resolve<Controller>().handle() == 20);
    }

    SECTION("Default-constructible service") {
        di::Container container;

        container.addService<EmptyService>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE_NOTHROW(scope.resolve<EmptyService>());
    }
}

TEST_CASE("DI service lifetimes are respected") {
    SECTION("Scoped services are reused within a scope") {
        di::Container container;

        container.addService<Counter>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        auto& first = scope.resolve<Counter>();
        auto& second = scope.resolve<Counter>();

        REQUIRE(&first == &second);

        first.value = 42;

        REQUIRE(second.value == 42);
    }

    SECTION("Scoped services differ between scopes") {
        di::Container container;

        container.addService<Counter>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        REQUIRE(&firstScope.resolve<Counter>() != &secondScope.resolve<Counter>());
    }

    SECTION("Singleton services are reused across scopes") {
        di::Container container;

        container.addService<Counter>(di::ServiceLifetime::Singleton);

        container.finalizeRegistrations();

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        auto& first = firstScope.resolve<Counter>();
        auto& second = secondScope.resolve<Counter>();

        REQUIRE(&first == &second);

        first.value = 42;

        REQUIRE(second.value == 42);
    }

    SECTION("Transient services create new instances") {
        di::Container container;

        container.addService<Counter>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE(&scope.resolve<Counter>() != &scope.resolve<Counter>());
    }
}

TEST_CASE("DI preserves dependency lifetimes through injection") {
    SECTION("Scoped dependency is shared between consumers") {
        di::Container container;

        container.addService<Dependency>(di::ServiceLifetime::Scoped);
        container.addService<FirstConsumer, Dependency>(di::ServiceLifetime::Transient);
        container.addService<SecondConsumer, Dependency>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        auto& first = scope.resolve<FirstConsumer>();
        auto& second = scope.resolve<SecondConsumer>();

        REQUIRE(&first.dependency == &second.dependency);
    }

    SECTION("Singleton dependency is shared across scopes") {
        di::Container container;

        container.addService<Dependency>(di::ServiceLifetime::Singleton);
        container.addService<FirstConsumer, Dependency>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        auto& first = firstScope.resolve<FirstConsumer>();
        auto& second = secondScope.resolve<FirstConsumer>();

        REQUIRE(&first.dependency == &second.dependency);
    }

    SECTION("Transient dependency differs between resolutions") {
        di::Container container;

        container.addService<Dependency>(di::ServiceLifetime::Transient);
        container.addService<FirstConsumer, Dependency>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        auto& first = scope.resolve<FirstConsumer>();
        auto& second = scope.resolve<FirstConsumer>();

        REQUIRE(&first.dependency != &second.dependency);
    }
}

TEST_CASE("DI rejects invalid registrations and dependency graphs") {
    SECTION("Unregistered root service") {
        di::Container container;

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE_THROWS_AS(scope.resolve<Logger>(), std::logic_error);
    }

    SECTION("Missing dependency") {
        di::Container container;

        container.addService<RequiresMissingDependency, MissingDependency>(
            di::ServiceLifetime::Scoped);

        try {
            container.finalizeRegistrations();

            FAIL("Expected missing dependency validation to throw");
        } catch (const std::logic_error& exception) {
            REQUIRE(
                std::string_view(exception.what()).find("unregistered service") !=
                std::string_view::npos);
        }
    }

    SECTION("Duplicate registration") {
        di::Container container;

        container.addService<Logger>(di::ServiceLifetime::Scoped);

        REQUIRE_THROWS_AS(
            container.addService<Logger>(di::ServiceLifetime::Singleton),
            std::logic_error);
    }

    SECTION("Circular dependency") {
        di::Container container;

        container.addService<FirstCircularService, SecondCircularService>(
            di::ServiceLifetime::Transient);

        container.addService<SecondCircularService, FirstCircularService>(
            di::ServiceLifetime::Transient);

        REQUIRE_THROWS_AS(container.finalizeRegistrations(), std::logic_error);
    }
}

TEST_CASE("DI enforces lifetime dependency rules") {
    SECTION("Singleton cannot directly depend on scoped service") {
        di::Container container;

        container.addService<ScopedDependency>(di::ServiceLifetime::Scoped);
        container.addService<DirectScopedSingleton, ScopedDependency>(
            di::ServiceLifetime::Singleton);

        REQUIRE_THROWS_AS(container.finalizeRegistrations(), std::logic_error);
    }

    SECTION("Singleton cannot indirectly depend on scoped service") {
        di::Container container;

        container.addService<ScopedDependency>(di::ServiceLifetime::Scoped);
        container.addService<TransientScopedConsumer, ScopedDependency>(
            di::ServiceLifetime::Transient);
        container.addService<IndirectScopedSingleton, TransientScopedConsumer>(
            di::ServiceLifetime::Singleton);

        REQUIRE_THROWS_AS(container.finalizeRegistrations(), std::logic_error);
    }

    SECTION("Deep scoped dependency beneath singleton is rejected") {
        di::Container container;

        container.addService<ScopedDependency>(di::ServiceLifetime::Scoped);
        container.addService<TransientScopedConsumer, ScopedDependency>(
            di::ServiceLifetime::Transient);
        container.addService<MiddleTransientConsumer, TransientScopedConsumer>(
            di::ServiceLifetime::Transient);
        container.addService<DeepScopedSingleton, MiddleTransientConsumer>(
            di::ServiceLifetime::Singleton);

        REQUIRE_THROWS_AS(container.finalizeRegistrations(), std::logic_error);
    }

    SECTION("Singleton can depend on singleton") {
        di::Container container;

        container.addService<SingletonDependency>(di::ServiceLifetime::Singleton);
        container.addService<SingletonConsumer, SingletonDependency>(
            di::ServiceLifetime::Singleton);

        REQUIRE_NOTHROW(container.finalizeRegistrations());
    }

    SECTION("Singleton can depend on transient") {
        di::Container container;

        container.addService<TransientDependency>(di::ServiceLifetime::Transient);
        container.addService<SingletonWithTransient, TransientDependency>(
            di::ServiceLifetime::Singleton);

        REQUIRE_NOTHROW(container.finalizeRegistrations());
    }
}

TEST_CASE("DI construction failures preserve valid state") {
    SECTION("Scoped construction failure does not poison cache") {
        ThrowingService::reset();

        di::Container container;

        container.addService<ThrowingService>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE_THROWS_AS(scope.resolve<ThrowingService>(), std::runtime_error);

        ThrowingService::allowConstruction();

        auto& first = scope.resolve<ThrowingService>();
        auto& second = scope.resolve<ThrowingService>();

        REQUIRE(&first == &second);
        REQUIRE(ThrowingService::constructionAttempts() == 2);
    }

    SECTION("Singleton construction failure does not poison cache") {
        ThrowingService::reset();

        di::Container container;

        container.addService<ThrowingService>(di::ServiceLifetime::Singleton);

        container.finalizeRegistrations();

        auto firstScope = container.createScope();

        REQUIRE_THROWS_AS(firstScope.resolve<ThrowingService>(), std::runtime_error);

        ThrowingService::allowConstruction();

        auto secondScope = container.createScope();

        auto& first = firstScope.resolve<ThrowingService>();
        auto& second = secondScope.resolve<ThrowingService>();

        REQUIRE(&first == &second);
        REQUIRE(ThrowingService::constructionAttempts() == 2);
    }

    SECTION("Failed consumer construction preserves dependency cache") {
        CachedDependency::reset();
        ThrowingConsumer::reset();

        di::Container container;

        container.addService<CachedDependency>(di::ServiceLifetime::Scoped);
        container.addService<ThrowingConsumer, CachedDependency>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE_THROWS_AS(scope.resolve<ThrowingConsumer>(), std::runtime_error);

        ThrowingConsumer::allowConstruction();

        auto& first = scope.resolve<ThrowingConsumer>();
        auto& second = scope.resolve<ThrowingConsumer>();

        REQUIRE(&first == &second);
        REQUIRE(CachedDependency::constructions() == 1);
        REQUIRE(ThrowingConsumer::constructionAttempts() == 2);
    }
}

TEST_CASE("DI manages service resource lifetimes") {
    SECTION("Scoped service is destroyed with scope") {
        TrackedScopedService::reset();

        di::Container container;

        container.addService<TrackedScopedService>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        {
            auto scope = container.createScope();

            scope.resolve<TrackedScopedService>();

            REQUIRE(TrackedScopedService::aliveCount() == 1);
        }

        REQUIRE(TrackedScopedService::aliveCount() == 0);
    }

    SECTION("Singleton is destroyed with container") {
        TrackedSingleton::reset();

        {
            di::Container container;

            container.addService<TrackedSingleton>(di::ServiceLifetime::Singleton);

            container.finalizeRegistrations();

            auto scope = container.createScope();

            scope.resolve<TrackedSingleton>();

            REQUIRE(TrackedSingleton::aliveCount() == 1);
        }

        REQUIRE(TrackedSingleton::aliveCount() == 0);
    }

    SECTION("Unused scope constructs no services") {
        TrackedScopedService::reset();

        di::Container container;

        container.addService<TrackedScopedService>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        {
            auto scope = container.createScope();
        }

        REQUIRE(TrackedScopedService::aliveCount() == 0);
    }
}

TEST_CASE("DI supports immovable services") {
    SECTION("Transient") {
        di::Container container;

        container.addService<ImmovableService>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE(&scope.resolve<ImmovableService>() != &scope.resolve<ImmovableService>());
    }

    SECTION("Scoped") {
        di::Container container;

        container.addService<ImmovableService>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE(&scope.resolve<ImmovableService>() == &scope.resolve<ImmovableService>());
    }

    SECTION("Singleton") {
        di::Container container;

        container.addService<ImmovableService>(di::ServiceLifetime::Singleton);

        container.finalizeRegistrations();

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        REQUIRE(
            &firstScope.resolve<ImmovableService>() == &secondScope.resolve<ImmovableService>());
    }

    SECTION("Injection by reference") {
        di::Container container;

        container.addService<ImmovableService>(di::ServiceLifetime::Scoped);
        container.addService<ImmovableConsumer, ImmovableService>(di::ServiceLifetime::Transient);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        auto& service = scope.resolve<ImmovableService>();
        auto& consumer = scope.resolve<ImmovableConsumer>();

        REQUIRE(&consumer.service == &service);
    }
}

TEST_CASE("DI protects reserved internal services") {
    SECTION("Router cannot be registered by user") {
        di::Container container;

        container.reserveInternal<Router>();

        REQUIRE_THROWS_AS(
            container.addService<Router>(di::ServiceLifetime::Singleton),
            std::logic_error);
    }

    SECTION("BodyBinder cannot be registered by user") {
        di::Container container;

        container.reserveInternal<BodyBinder>();

        REQUIRE_THROWS_AS(
            container.addService<BodyBinder>(di::ServiceLifetime::Singleton),
            std::logic_error);
    }
}

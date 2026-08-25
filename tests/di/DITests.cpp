#include <catch2/catch_test_macros.hpp>

#include <string_view>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;

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

    struct Counter {
        int value = 0;
    };

    struct CounterConsumer {
        explicit CounterConsumer(Counter& counter) : counter(counter) {}

        Counter& counter;
    };

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

    struct EmptyService {};
}

TEST_CASE("DI resolves registered services and dependency graphs") {
    SECTION("Simple service") {
        di::Container container;
        container.addService<Logger>(di::ServiceLifetime::Scoped);
        container.finalizeRegistrations();

        auto scope = container.createScope();

        REQUIRE(scope.resolve<Logger>().value == 10);
    }

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

    SECTION("Service with empty constructor") {
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

        auto& first = firstScope.resolve<Counter>();
        auto& second = secondScope.resolve<Counter>();

        REQUIRE(&first != &second);

        first.value = 42;

        REQUIRE(second.value == 0);
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

        auto& first = scope.resolve<Counter>();
        auto& second = scope.resolve<Counter>();

        REQUIRE(&first != &second);

        first.value = 42;

        REQUIRE(second.value == 0);
    }
}

TEST_CASE("DI preserves dependency identity") {
    SECTION("Scoped dependencies") {
        di::Container container;

        container.addService<Logger>(di::ServiceLifetime::Scoped);
        container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
        container.addService<Service, Repository, Logger>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        auto& logger = scope.resolve<Logger>();
        auto& service = scope.resolve<Service>();

        REQUIRE(&service.logger == &logger);
        REQUIRE(&service.repository.logger == &logger);
    }

    SECTION("Injected references observe mutations") {
        di::Container container;

        container.addService<Counter>(di::ServiceLifetime::Scoped);
        container.addService<CounterConsumer, Counter>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        auto& counter = scope.resolve<Counter>();
        auto& consumer = scope.resolve<CounterConsumer>();

        counter.value = 42;

        REQUIRE(consumer.counter.value == 42);

        consumer.counter.value = 100;

        REQUIRE(counter.value == 100);
    }
}

TEST_CASE("DI rejects invalid resolutions and registrations") {
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

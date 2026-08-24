#include <memory>
#include <string>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

#include "support/Testing.hpp"

namespace di = mach::detail::di;

namespace test_services
{

    struct Logger {
        int value = 10;

        std::string log() const {
            return "logged";
        }
    };

    struct Repository {
        Logger& logger;

        explicit Repository(Logger& logger) : logger(logger) {}

        int query() const {
            return logger.value + 5;
        }
    };

    struct Service {
        Repository& repository;
        Logger& logger;

        Service(Repository& repository, Logger& logger) : repository(repository), logger(logger) {}

        int execute() const {
            return repository.query() + logger.value;
        }
    };

    struct Controller {
        Service& service;

        explicit Controller(Service& service) : service(service) {}

        int handle() const {
            return service.execute();
        }
    };

    struct Counter {
        int value = 0;

        void increment() {
            ++value;
        }
    };

    struct UsesMissingDependency {
        Repository& repository;

        explicit UsesMissingDependency(Repository& repository) : repository(repository) {}
    };

    struct CounterConsumer {
        Counter& counter;

        explicit CounterConsumer(Counter& counter) : counter(counter) {}

        int read() const {
            return counter.value;
        }
    };

    struct EmptyCtor {};

} // namespace test_services

static void runDiContainerTests() {
    using namespace test_services;

    {
        const std::string testName = "DI resolves simple scoped service";

        try {
            di::Container container;
            container.addService<Logger>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();
            auto& logger = scope.resolve<Logger>();

            if (logger.log() != "logged") {
                testing::fail(testName, "Resolved Logger is not usable at runtime");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "DI resolves dependency graph with references";

        try {
            di::Container container;
            container.addService<Logger>(di::ServiceLifetime::Scoped);
            container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
            container.addService<Service, Repository, Logger>(di::ServiceLifetime::Scoped);
            container.addService<Controller, Service>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();
            auto& controller = scope.resolve<Controller>();

            if (controller.handle() != 25) {
                testing::fail(testName, "Resolved object graph produced wrong runtime result");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Registration order does not matter";

        try {
            di::Container container;

            container.addService<Controller, Service>(di::ServiceLifetime::Scoped);
            container.addService<Service, Repository, Logger>(di::ServiceLifetime::Scoped);
            container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
            container.addService<Logger>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();
            auto& controller = scope.resolve<Controller>();

            if (controller.handle() != 25) {
                testing::fail(
                    testName,
                    "Container failed to resolve graph registered in reverse order");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Scoped services are reused inside same scope";

        try {
            di::Container container;
            container.addService<Counter>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();

            auto& first = scope.resolve<Counter>();
            auto& second = scope.resolve<Counter>();

            if (&first != &second) {
                testing::fail(
                    testName,
                    "Scoped service returned different instances in same scope");
                return;
            }

            first.increment();

            if (second.value != 1) {
                testing::fail(testName, "Scoped service did not preserve state inside same scope");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Scoped services differ between scopes";

        try {
            di::Container container;
            container.addService<Counter>(di::ServiceLifetime::Scoped);

            auto scopeA = container.createScope();
            auto scopeB = container.createScope();

            auto& first = scopeA.resolve<Counter>();
            auto& second = scopeB.resolve<Counter>();

            if (&first == &second) {
                testing::fail(
                    testName,
                    "Scoped service reused same instance across different scopes");
                return;
            }

            first.increment();

            if (second.value != 0) {
                testing::fail(testName, "Scoped state leaked across scopes");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Singleton services are reused across scopes";

        try {
            di::Container container;
            container.addService<Counter>(di::ServiceLifetime::Singleton);

            auto scopeA = container.createScope();
            auto scopeB = container.createScope();

            auto& first = scopeA.resolve<Counter>();
            auto& second = scopeB.resolve<Counter>();

            if (&first != &second) {
                testing::fail(
                    testName,
                    "Singleton service returned different instances across scopes");
                return;
            }

            first.increment();

            if (second.value != 1) {
                testing::fail(testName, "Singleton state was not shared across scopes");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Transient services create new instances";

        try {
            di::Container container;
            container.addService<Counter>(di::ServiceLifetime::Transient);

            auto scope = container.createScope();

            auto& first = scope.resolve<Counter>();
            auto& second = scope.resolve<Counter>();

            if (&first == &second) {
                testing::fail(testName, "Transient service reused the same instance");
                return;
            }

            first.increment();

            if (second.value != 0) {
                testing::fail(testName, "Transient state leaked between instances");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Missing root service throws";

        try {
            di::Container container;

            auto scope = container.createScope();
            auto& logger = scope.resolve<Logger>();

            (void)logger;

            testing::fail(testName, "Resolving an unregistered service did not throw");
        } catch (const std::exception&) {
            testing::success(testName);
        }
    }

    {
        const std::string testName = "Missing nested dependency throws";

        try {
            di::Container container;

            container.addService<UsesMissingDependency, Repository>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();
            auto& service = scope.resolve<UsesMissingDependency>();

            (void)service;

            testing::fail(testName, "Resolving service with missing dependency did not throw");
        } catch (const std::exception&) {
            testing::success(testName);
        }
    }

    {
        const std::string testName = "Duplicate registration throws";

        try {
            di::Container container;

            container.addService<Logger>(di::ServiceLifetime::Scoped);
            container.addService<Logger>(di::ServiceLifetime::Singleton);

            testing::fail(testName, "Duplicate registration did not throw");
        } catch (const std::exception&) {
            testing::success(testName);
        }
    }

    {
        const std::string testName = "Empty constructor service resolves";

        try {
            di::Container container;
            container.addService<EmptyCtor>(di::ServiceLifetime::Transient);

            auto scope = container.createScope();
            auto& service = scope.resolve<EmptyCtor>();

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Scoped dependency identity is preserved inside graph";

        try {
            di::Container container;

            container.addService<Logger>(di::ServiceLifetime::Scoped);
            container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
            container.addService<Service, Repository, Logger>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();

            auto& logger = scope.resolve<Logger>();
            auto& service = scope.resolve<Service>();

            if (&service.logger != &logger) {
                testing::fail(
                    testName,
                    "Injected Logger reference does not point to scoped Logger instance");
                return;
            }

            if (&service.repository.logger != &logger) {
                testing::fail(
                    testName,
                    "Nested injected Logger reference does not point to scoped Logger instance");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Singleton dependency identity is preserved inside graph";

        try {
            di::Container container;

            container.addService<Counter>(di::ServiceLifetime::Singleton);

            auto scopeA = container.createScope();
            auto scopeB = container.createScope();

            auto& first = scopeA.resolve<Counter>();
            auto& second = scopeB.resolve<Counter>();

            first.value = 123;

            if (second.value != 123) {
                testing::fail(testName, "Singleton mutation was not visible from another scope");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Injected reference observes scoped mutations";

        try {
            di::Container container;

            container.addService<Counter>(di::ServiceLifetime::Scoped);
            container.addService<CounterConsumer, Counter>(di::ServiceLifetime::Scoped);

            auto scope = container.createScope();

            auto& counter = scope.resolve<Counter>();
            auto& consumer = scope.resolve<CounterConsumer>();

            counter.value = 42;

            if (consumer.read() != 42) {
                testing::fail(
                    testName,
                    "Injected reference did not observe mutation on scoped instance");
                return;
            }

            consumer.counter.value = 100;

            if (counter.value != 100) {
                testing::fail(
                    testName,
                    "Mutation through injected reference did not affect scoped instance");
                return;
            }

            testing::success(testName);
        } catch (const std::exception& ex) {
            testing::fail(testName, ex.what());
        }
    }

    std::cout << "\n";
    testing::success("All container tests");
}

int main() {
    runDiContainerTests();

    return 0;
}

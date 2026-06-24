#include <string>
#include <string_view>
#include <stdexcept>
#include <memory>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

#include "Testing.hpp"

namespace di = mach::detail::di;

namespace test_services {

    struct Logger {
        int value = 10;

        std::string log() const {
            return "logged";
        }
    };

    struct Repository {
        Logger& logger;

        explicit Repository(Logger& logger)
            : logger(logger) {}

        int query() const {
            return logger.value + 5;
        }
    };

    struct Service {
        Repository& repository;
        Logger& logger;

        Service(Repository& repository, Logger& logger)
            : repository(repository), logger(logger) {}

        int execute() const {
            return repository.query() + logger.value;
        }
    };

    struct Controller {
        Service& service;

        explicit Controller(Service& service)
            : service(service) {}

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

        explicit UsesMissingDependency(Repository& repository)
            : repository(repository) {}
    };

    struct CounterConsumer {
        Counter& counter;

        explicit CounterConsumer(Counter& counter)
            : counter(counter) {}

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
                test::fail(testName, "Resolved Logger is not usable at runtime");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Resolved object graph produced wrong runtime result");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Container failed to resolve graph registered in reverse order");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Scoped service returned different instances in same scope");
                return;
            }

            first.increment();

            if (second.value != 1) {
                test::fail(testName, "Scoped service did not preserve state inside same scope");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Scoped service reused same instance across different scopes");
                return;
            }

            first.increment();

            if (second.value != 0) {
                test::fail(testName, "Scoped state leaked across scopes");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Singleton service returned different instances across scopes");
                return;
            }

            first.increment();

            if (second.value != 1) {
                test::fail(testName, "Singleton state was not shared across scopes");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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

            if (&first != &second) {
                test::fail(testName, "Transient service reused the same instance");
                return;
            }

            first.increment();

            if (second.value != 0) {
                test::fail(testName, "Transient state leaked between instances");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
        }
    }

    {
        const std::string testName = "Missing root service throws";

        try {
            di::Container container;

            auto scope = container.createScope();
            auto& logger = scope.resolve<Logger>();

            (void)logger;

            test::fail(testName, "Resolving an unregistered service did not throw");
        }
        catch (const std::exception&) {
            test::success(testName);
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

            test::fail(testName, "Resolving service with missing dependency did not throw");
        }
        catch (const std::exception&) {
            test::success(testName);
        }
    }

    {
        const std::string testName = "Duplicate registration throws";

        try {
            di::Container container;

            container.addService<Logger>(di::ServiceLifetime::Scoped);
            container.addService<Logger>(di::ServiceLifetime::Singleton);

            test::fail(testName, "Duplicate registration did not throw");
        }
        catch (const std::exception&) {
            test::success(testName);
        }
    }

    {
        const std::string testName = "Empty constructor service resolves";

        try {
            di::Container container;
            container.addService<EmptyCtor>(di::ServiceLifetime::Transient);

            auto scope = container.createScope();
            auto& service = scope.resolve<EmptyCtor>();

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Injected Logger reference does not point to scoped Logger instance");
                return;
            }

            if (&service.repository.logger != &logger) {
                test::fail(testName, "Nested injected Logger reference does not point to scoped Logger instance");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Singleton mutation was not visible from another scope");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
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
                test::fail(testName, "Injected reference did not observe mutation on scoped instance");
                return;
            }

            consumer.counter.value = 100;

            if (counter.value != 100) {
                test::fail(testName, "Mutation through injected reference did not affect scoped instance");
                return;
            }

            test::success(testName);
        }
        catch (const std::exception& ex) {
            test::fail(testName, ex.what());
        }
    }

    std::cout << "\n";
    test::success("All container tests");
}

int main() {
    runDiContainerTests();


	return 0;
}

#include <cassert>
#include <stdexcept>
#include <string>
#include <string_view>

#include <mach/Reply.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/diagnostics/TerminateHandler.hpp>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>

#include "Testing.hpp"

struct ScopedService {};

struct TestController : mach::ControllerBase {
    static inline std::string route = "/test";

    explicit TestController(ScopedService& service) : service(&service) {}

    static void configure(mach::ControllerBuilder<TestController>& routes) {}

    ScopedService* service;
};

struct FlakyScopedDependency {
    inline static int constructionAttempts = 0;

    FlakyScopedDependency() {
        ++constructionAttempts;

        if (constructionAttempts == 1) {
            throw std::runtime_error("boom");
        }
    }
};

struct ThrowingController : mach::ControllerBase {
    static inline std::string route = "/throwing";

    explicit ThrowingController(FlakyScopedDependency& dependency) : dependency(&dependency) {}

    static void configure(mach::ControllerBuilder<ThrowingController>& routes) {}

    FlakyScopedDependency* dependency;
};

struct Logger {};

struct TestController2 : mach::ControllerBase {
    explicit TestController2(ScopedService&) : constructorUsed(1) {}

    TestController2(ScopedService&, Logger&) : constructorUsed(2) {}

    int constructorUsed;
};

namespace di = mach::detail::di;

int main() {
    mach::installTerminateHandler();

    di::Container container;

    container.addService<ScopedService>(di::ServiceLifetime::Scoped);

    container.addService<TestController, ScopedService>(
        di::ServiceLifetime::Transient,
        di::ServiceAccess::Internal);

    container.addService<FlakyScopedDependency>(di::ServiceLifetime::Scoped);

    container.addService<ThrowingController, FlakyScopedDependency>(
        di::ServiceLifetime::Transient,
        di::ServiceAccess::Internal);

    container.addService<Logger>(di::ServiceLifetime::Singleton);

    container.addService<TestController2, ScopedService, Logger>(
        di::ServiceLifetime::Transient,
        di::ServiceAccess::Internal);

    container.finalizeRegistrations();

    {
        auto scope = container.createScope();

        auto& controller1 = scope.resolve<TestController>();
        auto& controller2 = scope.resolve<TestController>();

        assert(&controller1 != &controller2);
        assert(controller1.service == controller2.service);

        testing::success("Transient controllers differ within the same scope");

        testing::success("Scoped controller dependency is shared within the same scope");
    }

    {
        auto scope1 = container.createScope();
        auto scope2 = container.createScope();

        auto& controller1 = scope1.resolve<TestController>();
        auto& controller2 = scope2.resolve<TestController>();

        assert(controller1.service != controller2.service);

        testing::success("Scoped controller dependency differs between scopes");
    }

    {
        FlakyScopedDependency::constructionAttempts = 0;

        auto scope = container.createScope();

        bool exceptionThrown = false;

        try {
            scope.resolve<ThrowingController>();
        } catch (const std::runtime_error& exception) {
            exceptionThrown = std::string_view(exception.what()) == "boom";
        }

        assert(exceptionThrown);
        assert(FlakyScopedDependency::constructionAttempts == 1);

        auto& controller = scope.resolve<ThrowingController>();

        assert(controller.dependency != nullptr);
        assert(FlakyScopedDependency::constructionAttempts == 2);

        auto& secondController = scope.resolve<ThrowingController>();

        assert(&controller != &secondController);
        assert(controller.dependency == secondController.dependency);

        assert(FlakyScopedDependency::constructionAttempts == 2);

        testing::success("Controller construction exception propagates");

        testing::success("Failed scoped dependency construction does not poison the scope cache");

        testing::success("Scoped dependency is cached after successful retry");
    }

    {
        auto scope = container.createScope();

        auto& controller = scope.resolve<TestController2>();

        assert(controller.constructorUsed == 2);

        testing::success("Container uses the correct constructor for resolution");
    }

    return 0;
}
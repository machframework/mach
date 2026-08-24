#include <stdexcept>
#include <string_view>

#include "support/Testing.hpp"

#include <mach/detail/binding/BodyBinder.hpp>
#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>
#include <mach/detail/routing/Router.hpp>

namespace
{
    namespace di = mach::detail::di;

    using mach::detail::binding::BodyBinder;
    using mach::detail::routing::Router;

    class RouterConsumer {
    public:
        explicit RouterConsumer(Router& router) noexcept : m_router(router) {}

    private:
        Router& m_router;
    };

    class BodyBinderConsumer {
    public:
        explicit BodyBinderConsumer(BodyBinder& bodyBinder) noexcept : m_bodyBinder(bodyBinder) {}

    private:
        BodyBinder& m_bodyBinder;
    };

    void testRouterCannotBeRegisteredByUser() {
        constexpr std::string_view testName = "Router cannot be registered by user";

        di::Container container;
        container.reserveInternal<Router>();
        container.reserveInternal<BodyBinder>();

        try {
            container.addService<Router>(di::ServiceLifetime::Singleton);

            testing::fail(testName, "Reserved Router registration unexpectedly succeeded");
        } catch (const std::logic_error&) {
            testing::success(testName);
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }

    void testBodyBinderCannotBeRegisteredByUser() {
        constexpr std::string_view testName = "BodyBinder cannot be registered by user";

        di::Container container;
        container.reserveInternal<Router>();
        container.reserveInternal<BodyBinder>();

        try {
            container.addService<BodyBinder>(di::ServiceLifetime::Singleton);

            testing::fail(testName, "Reserved BodyBinder registration unexpectedly succeeded");
        } catch (const std::logic_error&) {
            testing::success(testName);
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }
}

int main() {
    testRouterCannotBeRegisteredByUser();
    testBodyBinderCannotBeRegisteredByUser();

    return 0;
}
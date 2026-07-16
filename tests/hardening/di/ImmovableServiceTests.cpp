#include <string_view>

#include "Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>


namespace
{
    namespace di = mach::detail::di;


    class ImmovableService
    {
    public:
        ImmovableService() = default;

        ImmovableService(const ImmovableService&) = delete;
        ImmovableService& operator=(
            const ImmovableService&
            ) = delete;

        ImmovableService(ImmovableService&&) = delete;
        ImmovableService& operator=(
            ImmovableService&&
            ) = delete;

        [[nodiscard]]
        int value() const noexcept
        {
            return 42;
        }
    };


    class Consumer
    {
    public:
        explicit Consumer(
            ImmovableService& service
        ) noexcept
            : m_service(service)
        {}

        [[nodiscard]]
        ImmovableService& service() const noexcept
        {
            return m_service;
        }

    private:
        ImmovableService& m_service;
    };


    void testTransientImmovableServiceCanBeResolved()
    {
        constexpr std::string_view testName =
            "Transient immovable service can be resolved";

        di::Container container;

        container.addService<ImmovableService>(
            di::ServiceLifetime::Transient
        );

        container.finalizeRegistrations();

        auto scope = container.createScope();

        try {
            auto& first =
                scope.resolve<ImmovableService>();

            auto& second =
                scope.resolve<ImmovableService>();

            if (first.value() != 42 || second.value() != 42) {
                testing::fail(
                    testName,
                    "Resolved service returned an unexpected value"
                );

                return;
            }

            if (&first == &second) {
                testing::fail(
                    testName,
                    "Transient resolutions returned the same instance"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testScopedImmovableServiceCanBeResolved()
    {
        constexpr std::string_view testName =
            "Scoped immovable service can be resolved";

        di::Container container;

        container.addService<ImmovableService>(
            di::ServiceLifetime::Scoped
        );

        container.finalizeRegistrations();

        auto scope = container.createScope();

        try {
            auto& first =
                scope.resolve<ImmovableService>();

            auto& second =
                scope.resolve<ImmovableService>();

            if (first.value() != 42 || second.value() != 42) {
                testing::fail(
                    testName,
                    "Resolved service returned an unexpected value"
                );

                return;
            }

            if (&first != &second) {
                testing::fail(
                    testName,
                    "Scoped resolutions returned different instances"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonImmovableServiceCanBeResolved()
    {
        constexpr std::string_view testName =
            "Singleton immovable service can be resolved";

        di::Container container;

        container.addService<ImmovableService>(
            di::ServiceLifetime::Singleton
        );

        container.finalizeRegistrations();

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        try {
            auto& first =
                firstScope.resolve<ImmovableService>();

            auto& second =
                secondScope.resolve<ImmovableService>();

            if (first.value() != 42 || second.value() != 42) {
                testing::fail(
                    testName,
                    "Resolved service returned an unexpected value"
                );

                return;
            }

            if (&first != &second) {
                testing::fail(
                    testName,
                    "Singleton resolutions returned different instances"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testImmovableServiceCanBeInjectedByReference()
    {
        constexpr std::string_view testName =
            "Immovable service can be injected by reference";

        di::Container container;

        container.addService<ImmovableService>(
            di::ServiceLifetime::Scoped
        );

        container.addService<
            Consumer,
            ImmovableService
        >(
            di::ServiceLifetime::Transient
        );

        container.finalizeRegistrations();

        auto scope = container.createScope();

        try {
            auto& service =
                scope.resolve<ImmovableService>();

            auto& consumer =
                scope.resolve<Consumer>();

            if (&consumer.service() != &service) {
                testing::fail(
                    testName,
                    "Consumer received a different service instance"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }
}


int main()
{
    testTransientImmovableServiceCanBeResolved();
    testScopedImmovableServiceCanBeResolved();
    testSingletonImmovableServiceCanBeResolved();
    testImmovableServiceCanBeInjectedByReference();

    return 0;
}

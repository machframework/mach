#include <stdexcept>
#include <string_view>

#include "Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;

    class Dependency
    {};

    class FirstConsumer
    {
    public:
        explicit FirstConsumer(Dependency& dependency) noexcept
            : m_dependency(dependency)
        {}

        [[nodiscard]]
        Dependency& dependency() const noexcept
        {
            return m_dependency;
        }

    private:
        Dependency& m_dependency;
    };

    class SecondConsumer
    {
    public:
        explicit SecondConsumer(Dependency& dependency) noexcept
            : m_dependency(dependency)
        {}

        [[nodiscard]]
        Dependency& dependency() const noexcept
        {
            return m_dependency;
        }

    private:
        Dependency& m_dependency;
    };

    class ThrowingService
    {
    public:
        ThrowingService()
        {
            ++s_constructionAttempts;

            if (s_shouldThrow) {
                throw std::runtime_error(
                    "Intentional construction failure"
                );
            }
        }

        static void reset() noexcept
        {
            s_constructionAttempts = 0;
            s_shouldThrow = true;
        }

        static void allowConstruction() noexcept
        {
            s_shouldThrow = false;
        }

        [[nodiscard]]
        static int constructionAttempts() noexcept
        {
            return s_constructionAttempts;
        }

    private:
        inline static int s_constructionAttempts = 0;
        inline static bool s_shouldThrow = true;
    };


    void testScopedDependencySharedWithinScope()
    {
        constexpr std::string_view testName =
            "Scoped dependency shared between consumers in same scope";

        di::Container container;

        container.addService<Dependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<FirstConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        container.addService<SecondConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        auto scope = container.createScope();

        try {
            auto& firstConsumer =
                scope.resolve<FirstConsumer>();

            auto& secondConsumer =
                scope.resolve<SecondConsumer>();

            if (
                &firstConsumer.dependency() !=
                &secondConsumer.dependency()
                ) {
                testing::fail(
                    testName,
                    "Consumers received different scoped dependency instances"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testScopedDependencyDifferentAcrossScopes()
    {
        constexpr std::string_view testName =
            "Scoped dependency differs between scopes";

        di::Container container;

        container.addService<Dependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<FirstConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        try {
            auto& firstConsumer =
                firstScope.resolve<FirstConsumer>();

            auto& secondConsumer =
                secondScope.resolve<FirstConsumer>();

            if (
                &firstConsumer.dependency() ==
                &secondConsumer.dependency()
                ) {
                testing::fail(
                    testName,
                    "Consumers from different scopes received the same scoped dependency"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonDependencySharedWithinScope()
    {
        constexpr std::string_view testName =
            "Singleton dependency shared between consumers in same scope";

        di::Container container;

        container.addService<Dependency>(
            di::ServiceLifetime::Singleton
        );

        container.addService<FirstConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        container.addService<SecondConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        auto scope = container.createScope();

        try {
            auto& firstConsumer =
                scope.resolve<FirstConsumer>();

            auto& secondConsumer =
                scope.resolve<SecondConsumer>();

            if (
                &firstConsumer.dependency() !=
                &secondConsumer.dependency()
                ) {
                testing::fail(
                    testName,
                    "Consumers received different singleton dependency instances"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonDependencySharedAcrossScopes()
    {
        constexpr std::string_view testName =
            "Singleton dependency shared across scopes";

        di::Container container;

        container.addService<Dependency>(
            di::ServiceLifetime::Singleton
        );

        container.addService<FirstConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        try {
            auto& firstConsumer =
                firstScope.resolve<FirstConsumer>();

            auto& secondConsumer =
                secondScope.resolve<FirstConsumer>();

            if (
                &firstConsumer.dependency() !=
                &secondConsumer.dependency()
                ) {
                testing::fail(
                    testName,
                    "Consumers from different scopes received different singleton dependencies"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testTransientDependencyDifferentWithinScope()
    {
        constexpr std::string_view testName =
            "Transient dependency differs between resolutions in same scope";

        di::Container container;

        container.addService<Dependency>(
            di::ServiceLifetime::Transient
        );

        container.addService<FirstConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        auto scope = container.createScope();

        try {
            auto& firstConsumer =
                scope.resolve<FirstConsumer>();

            auto& secondConsumer =
                scope.resolve<FirstConsumer>();

            if (
                &firstConsumer.dependency() ==
                &secondConsumer.dependency()
                ) {
                testing::fail(
                    testName,
                    "Separate consumer resolutions received the same transient dependency"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testTransientDependencyDifferentAcrossScopes()
    {
        constexpr std::string_view testName =
            "Transient dependency differs across scopes";

        di::Container container;

        container.addService<Dependency>(
            di::ServiceLifetime::Transient
        );

        container.addService<FirstConsumer, Dependency>(
            di::ServiceLifetime::Transient
        );

        auto firstScope = container.createScope();
        auto secondScope = container.createScope();

        try {
            auto& firstConsumer =
                firstScope.resolve<FirstConsumer>();

            auto& secondConsumer =
                secondScope.resolve<FirstConsumer>();

            if (
                &firstConsumer.dependency() ==
                &secondConsumer.dependency()
                ) {
                testing::fail(
                    testName,
                    "Consumers from different scopes received the same transient dependency"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testScopedConstructionFailureDoesNotPoisonCache()
    {
        constexpr std::string_view testName =
            "Scoped construction failure does not poison cache";

        ThrowingService::reset();

        di::Container container;

        container.addService<ThrowingService>(
            di::ServiceLifetime::Scoped
        );

        auto scope = container.createScope();

        try {
            [[maybe_unused]]
            auto& service = scope.resolve<ThrowingService>();

            testing::fail(
                testName,
                "First scoped resolution unexpectedly succeeded"
            );

            return;
        }
        catch (const std::runtime_error&) {
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
            return;
        }

        ThrowingService::allowConstruction();

        try {
            auto& firstSuccessfulResolution =
                scope.resolve<ThrowingService>();

            auto& secondSuccessfulResolution =
                scope.resolve<ThrowingService>();

            if (
                &firstSuccessfulResolution !=
                &secondSuccessfulResolution
                ) {
                testing::fail(
                    testName,
                    "Successful scoped instance was not cached"
                );

                return;
            }

            if (ThrowingService::constructionAttempts() != 2) {
                testing::fail(
                    testName,
                    "Scoped service was constructed an unexpected number of times"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonConstructionFailureDoesNotPoisonCache()
    {
        constexpr std::string_view testName =
            "Singleton construction failure does not poison cache";

        ThrowingService::reset();

        di::Container container;

        container.addService<ThrowingService>(
            di::ServiceLifetime::Singleton
        );

        auto firstScope = container.createScope();

        try {
            [[maybe_unused]]
            auto& service =
                firstScope.resolve<ThrowingService>();

            testing::fail(
                testName,
                "First singleton resolution unexpectedly succeeded"
            );

            return;
        }
        catch (const std::runtime_error&) {
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
            return;
        }

        ThrowingService::allowConstruction();

        auto secondScope = container.createScope();

        try {
            auto& firstSuccessfulResolution =
                firstScope.resolve<ThrowingService>();

            auto& secondSuccessfulResolution =
                secondScope.resolve<ThrowingService>();

            if (
                &firstSuccessfulResolution !=
                &secondSuccessfulResolution
                ) {
                testing::fail(
                    testName,
                    "Successful singleton instance was not shared across scopes"
                );

                return;
            }

            if (ThrowingService::constructionAttempts() != 2) {
                testing::fail(
                    testName,
                    "Singleton service was constructed an unexpected number of times"
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
    testScopedDependencySharedWithinScope();
    testScopedDependencyDifferentAcrossScopes();

    testSingletonDependencySharedWithinScope();
    testSingletonDependencySharedAcrossScopes();

    testTransientDependencyDifferentWithinScope();
    testTransientDependencyDifferentAcrossScopes();

    testScopedConstructionFailureDoesNotPoisonCache();
    testSingletonConstructionFailureDoesNotPoisonCache();

    return 0;
}
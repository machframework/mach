#include <stdexcept>
#include <string_view>

#include "Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;


    class ScopedDependency
    {};


    class SingletonDependency
    {};


    class TransientDependency
    {};


    class DirectScopedConsumer
    {
    public:
        explicit DirectScopedConsumer(
            ScopedDependency& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        ScopedDependency& m_dependency;
    };


    class TransientScopedConsumer
    {
    public:
        explicit TransientScopedConsumer(
            ScopedDependency& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        ScopedDependency& m_dependency;
    };


    class IndirectScopedConsumer
    {
    public:
        explicit IndirectScopedConsumer(
            TransientScopedConsumer& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        TransientScopedConsumer& m_dependency;
    };


    class MiddleTransientConsumer
    {
    public:
        explicit MiddleTransientConsumer(
            TransientScopedConsumer& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        TransientScopedConsumer& m_dependency;
    };


    class DeepScopedConsumer
    {
    public:
        explicit DeepScopedConsumer(
            MiddleTransientConsumer& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        MiddleTransientConsumer& m_dependency;
    };


    class SingletonConsumer
    {
    public:
        explicit SingletonConsumer(
            SingletonDependency& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        SingletonDependency& m_dependency;
    };


    class TransientConsumer
    {
    public:
        explicit TransientConsumer(
            TransientDependency& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        TransientDependency& m_dependency;
    };


    class ScopedConsumer
    {
    public:
        explicit ScopedConsumer(
            ScopedDependency& dependency
        ) noexcept
            : m_dependency(dependency)
        {}

    private:
        ScopedDependency& m_dependency;
    };


    void testSingletonCannotDirectlyDependOnScoped()
    {
        constexpr std::string_view testName =
            "Singleton cannot directly depend on scoped service";

        di::Container container;

        container.addService<ScopedDependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<
            DirectScopedConsumer,
            ScopedDependency
        >(
            di::ServiceLifetime::Singleton
        );

        try {
            container.finalizeRegistrations();

            testing::fail(
                testName,
                "Finalization accepted a singleton with a scoped dependency"
            );
        }
        catch (const std::logic_error&) {
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonCannotIndirectlyDependOnScoped()
    {
        constexpr std::string_view testName =
            "Singleton cannot indirectly depend on scoped service";

        di::Container container;

        container.addService<ScopedDependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<
            TransientScopedConsumer,
            ScopedDependency
        >(
            di::ServiceLifetime::Transient
        );

        container.addService<
            IndirectScopedConsumer,
            TransientScopedConsumer
        >(
            di::ServiceLifetime::Singleton
        );

        try {
            container.finalizeRegistrations();

            testing::fail(
                testName,
                "Finalization accepted an indirect scoped dependency"
            );
        }
        catch (const std::logic_error&) {
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testDeepScopedDependencyIsRejected()
    {
        constexpr std::string_view testName =
            "Deep scoped dependency beneath singleton is rejected";

        di::Container container;

        container.addService<ScopedDependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<
            TransientScopedConsumer,
            ScopedDependency
        >(
            di::ServiceLifetime::Transient
        );

        container.addService<
            MiddleTransientConsumer,
            TransientScopedConsumer
        >(
            di::ServiceLifetime::Transient
        );

        container.addService<
            DeepScopedConsumer,
            MiddleTransientConsumer
        >(
            di::ServiceLifetime::Singleton
        );

        try {
            container.finalizeRegistrations();

            testing::fail(
                testName,
                "Finalization accepted a deeply nested scoped dependency"
            );
        }
        catch (const std::logic_error&) {
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonCanDependOnSingleton()
    {
        constexpr std::string_view testName =
            "Singleton can depend on singleton";

        di::Container container;

        container.addService<SingletonDependency>(
            di::ServiceLifetime::Singleton
        );

        container.addService<
            SingletonConsumer,
            SingletonDependency
        >(
            di::ServiceLifetime::Singleton
        );

        try {
            container.finalizeRegistrations();
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testSingletonCanDependOnTransient()
    {
        constexpr std::string_view testName =
            "Singleton can depend on transient without scoped dependencies";

        di::Container container;

        container.addService<TransientDependency>(
            di::ServiceLifetime::Transient
        );

        container.addService<
            TransientConsumer,
            TransientDependency
        >(
            di::ServiceLifetime::Singleton
        );

        try {
            container.finalizeRegistrations();
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testScopedCanDependOnScoped()
    {
        constexpr std::string_view testName =
            "Scoped service can depend on scoped service";

        di::Container container;

        container.addService<ScopedDependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<
            ScopedConsumer,
            ScopedDependency
        >(
            di::ServiceLifetime::Scoped
        );

        try {
            container.finalizeRegistrations();
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testTransientCanDependOnScoped()
    {
        constexpr std::string_view testName =
            "Transient service can depend on scoped service";

        di::Container container;

        container.addService<ScopedDependency>(
            di::ServiceLifetime::Scoped
        );

        container.addService<
            TransientScopedConsumer,
            ScopedDependency
        >(
            di::ServiceLifetime::Transient
        );

        try {
            container.finalizeRegistrations();
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }
}


int main()
{
    testSingletonCannotDirectlyDependOnScoped();
    testSingletonCannotIndirectlyDependOnScoped();
    testDeepScopedDependencyIsRejected();

    testSingletonCanDependOnSingleton();
    testSingletonCanDependOnTransient();
    testScopedCanDependOnScoped();
    testTransientCanDependOnScoped();

    return 0;
}

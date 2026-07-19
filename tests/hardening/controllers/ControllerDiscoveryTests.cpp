#include <exception>
#include <string_view>

#include "Testing.hpp"

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

namespace
{
    class FirstController final : public mach::ControllerBase
    {
    public:
        inline static constexpr std::string_view route =
            "/first";

        static void reset() noexcept
        {
            s_configurationCount = 0;
        }

        [[nodiscard]]
        static int configurationCount() noexcept
        {
            return s_configurationCount;
        }

        static void configure(
            mach::ControllerBuilder<FirstController>& builder
        )
        {
            ++s_configurationCount;

            builder.mapGet(
                "/",
                &FirstController::index
            );
        }

        mach::Reply<> index()
        {
            return noContent();
        }

    private:
        inline static int s_configurationCount = 0;
    };


    class SecondController final : public mach::ControllerBase
    {
    public:
        inline static constexpr std::string_view route =
            "/second";

        static void reset() noexcept
        {
            s_configurationCount = 0;
        }

        [[nodiscard]]
        static int configurationCount() noexcept
        {
            return s_configurationCount;
        }

        static void configure(
            mach::ControllerBuilder<SecondController>& builder
        )
        {
            ++s_configurationCount;

            builder.mapGet(
                "/",
                &SecondController::index
            );

            builder.mapGet(
                "/details",
                &SecondController::details
            );
        }

        mach::Reply<> index()
        {
            return noContent();
        }

        mach::Reply<> details()
        {
            return noContent();
        }

    private:
        inline static int s_configurationCount = 0;
    };


    class UnregisteredController final : public mach::ControllerBase
    {
    public:
        inline static constexpr std::string_view route =
            "/unregistered";

        static void reset() noexcept
        {
            s_configurationCount = 0;
        }

        [[nodiscard]]
        static int configurationCount() noexcept
        {
            return s_configurationCount;
        }

        static void configure(
            mach::ControllerBuilder<UnregisteredController>& builder
        )
        {
            ++s_configurationCount;

            builder.mapGet(
                "/",
                &UnregisteredController::index
            );
        }

        mach::Reply<> index()
        {
            return noContent();
        }

    private:
        inline static int s_configurationCount = 0;
    };


    void resetControllers() noexcept
    {
        FirstController::reset();
        SecondController::reset();
        UnregisteredController::reset();
    }


    void testControllerIsNotMappedDuringRegistration()
    {
        constexpr std::string_view testName =
            "Controller is not mapped before application build";

        resetControllers();

        try {
            mach::AppBuilder builder(testing::serverOptions);

            builder.addController<FirstController>();

            if (FirstController::configurationCount() != 0) {
                testing::fail(
                    testName,
                    "Controller configure was called during registration"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testRegisteredControllerIsMappedDuringBuild()
    {
        constexpr std::string_view testName =
            "Registered controller is mapped during application build";

        resetControllers();

        try {
            mach::AppBuilder builder(testing::serverOptions);

            builder.addController<FirstController>();

            [[maybe_unused]]
            mach::App app = builder.build();

            if (FirstController::configurationCount() != 1) {
                testing::fail(
                    testName,
                    "Registered controller was not configured exactly once"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testMultipleRegisteredControllersAreMapped()
    {
        constexpr std::string_view testName =
            "Multiple registered controllers are mapped during build";

        resetControllers();

        try {
            mach::AppBuilder builder(testing::serverOptions);

            builder.addController<FirstController>();
            builder.addController<SecondController>();

            [[maybe_unused]]
            mach::App app = builder.build();

            if (FirstController::configurationCount() != 1) {
                testing::fail(
                    testName,
                    "First registered controller was not configured exactly once"
                );

                return;
            }

            if (SecondController::configurationCount() != 1) {
                testing::fail(
                    testName,
                    "Second registered controller was not configured exactly once"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testUnregisteredControllerIsNotMapped()
    {
        constexpr std::string_view testName =
            "Unregistered controller is not mapped during build";

        resetControllers();

        try {
            mach::AppBuilder builder(testing::serverOptions);

            builder.addController<FirstController>();
            builder.addController<SecondController>();

            [[maybe_unused]]
            mach::App app = builder.build();

            if (UnregisteredController::configurationCount() != 0) {
                testing::fail(
                    testName,
                    "Unregistered controller was unexpectedly configured"
                );

                return;
            }

            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }


    void testControllerWithMultipleActionsIsDiscovered()
    {
        constexpr std::string_view testName =
            "Controller with multiple actions is discovered successfully";

        resetControllers();

        try {
            mach::AppBuilder builder(testing::serverOptions);

            builder.addController<SecondController>();

            [[maybe_unused]]
            mach::App app = builder.build();

            if (SecondController::configurationCount() != 1) {
                testing::fail(
                    testName,
                    "Controller with multiple actions was not configured exactly once"
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
    testControllerIsNotMappedDuringRegistration();
    testRegisteredControllerIsMappedDuringBuild();
    testMultipleRegisteredControllersAreMapped();
    testUnregisteredControllerIsNotMapped();
    testControllerWithMultipleActionsIsDiscovered();

    return 0;
}
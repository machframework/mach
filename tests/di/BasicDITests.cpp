#include <string>
#include <string_view>

#include "support/Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;

    class Logger {
    public:
        void log(const std::string& value) {
            m_lastMessage = value;
        }

        [[nodiscard]]
        const std::string& lastMessage() const noexcept {
            return m_lastMessage;
        }

    private:
        std::string m_lastMessage;
    };

    class Db {
    public:
        explicit Db(Logger& logger) : m_logger(logger) {}

        [[nodiscard]]
        int query() const noexcept {
            return 40;
        }

    private:
        Logger& m_logger;
    };

    class UserService {
    public:
        explicit UserService(Db& userDb) : m_userDb(userDb) {}

    private:
        Db& m_userDb;
    };

    // Circular dependency:
    //
    // FirstCircularService
    //          ↓
    // SecondCircularService
    //          ↓
    // FirstCircularService

    class SecondCircularService;

    class FirstCircularService {
    public:
        explicit FirstCircularService(SecondCircularService& secondService)
            : m_secondService(secondService) {}

    private:
        SecondCircularService& m_secondService;
    };

    class SecondCircularService {
    public:
        explicit SecondCircularService(FirstCircularService& firstService)
            : m_firstService(firstService) {}

    private:
        FirstCircularService& m_firstService;
    };

    void testValidResolution() {
        constexpr std::string_view testName = "Valid scoped service resolution";

        di::Container container;

        container.addService<Logger>(di::ServiceLifetime::Scoped);

        auto scope = container.createScope();

        try {
            auto& logger = scope.resolve<Logger>();

            logger.log("Logging from a dynamically resolved logger!");

            if (logger.lastMessage() != "Logging from a dynamically resolved logger!") {
                testing::fail(testName, "Resolved service did not preserve its state");

                return;
            }

            testing::success(testName);
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }

    void testDuplicateRegistration() {
        constexpr std::string_view testName = "Duplicate service registration";

        di::Container container;

        container.addService<Logger>(di::ServiceLifetime::Scoped);

        try {
            container.addService<Logger>(di::ServiceLifetime::Scoped);

            testing::fail(testName, "Duplicate service registration did not throw");
        } catch (const std::logic_error& exception) {
            testing::success(testName);
            std::cout << "Got: " << exception.what() << '\n';
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }

    void testMissingDependency() {
        constexpr std::string_view testName = "Missing dependency registration";

        di::Container container;

        container.addService<UserService, Db>(di::ServiceLifetime::Scoped);

        auto scope = container.createScope();

        try {
            [[maybe_unused]]
            auto& userService = scope.resolve<UserService>();

            testing::fail(
                testName,
                "Resolving a service with an unregistered "
                "dependency did not throw");
        } catch (const std::logic_error& exception) {
            const std::string_view message = exception.what();

            if (message.find("dependency") == std::string_view::npos ||
                message.find("is not registered") == std::string_view::npos) {
                testing::fail(testName, exception.what());

                return;
            }

            testing::success(testName);
            std::cout << "Got: " << exception.what() << '\n';
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }

    void testCircularDependency() {
        constexpr std::string_view testName = "Circular dependency detection";

        di::Container container;

        container.addService<FirstCircularService, SecondCircularService>(
            di::ServiceLifetime::Transient);

        container.addService<SecondCircularService, FirstCircularService>(
            di::ServiceLifetime::Transient);

        auto scope = container.createScope();

        try {
            [[maybe_unused]]
            auto& service = scope.resolve<FirstCircularService>();

            testing::fail(testName, "Circular dependency did not throw");
        } catch (const std::logic_error& exception) {
            const std::string_view message = exception.what();

            if (message.find("circular dependency") == std::string_view::npos) {
                testing::fail(testName, exception.what());

                return;
            }

            testing::success(testName);
            std::cout << "Got: " << exception.what() << '\n';
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }

    void testScopeRecoveryAfterFailedResolution() {
        constexpr std::string_view testName = "Scope recovery after failed resolution";

        di::Container container;

        container.addService<Logger>(di::ServiceLifetime::Scoped);

        container.addService<UserService, Db>(di::ServiceLifetime::Scoped);

        auto scope = container.createScope();

        try {
            [[maybe_unused]]
            auto& userService = scope.resolve<UserService>();
        } catch (const std::logic_error&) {
        }

        try {
            auto& logger = scope.resolve<Logger>();

            logger.log("Scope remained usable after resolution failure");

            testing::success(testName);
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }
}

int main() {
    testValidResolution();
    testDuplicateRegistration();
    testMissingDependency();
    testCircularDependency();
    testScopeRecoveryAfterFailedResolution();

    return 0;
}
#include <stdexcept>
#include <string_view>

#include "support/Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;

    class Dependency {
    public:
        Dependency() {
            ++s_constructions;
        }

        static void reset() noexcept {
            s_constructions = 0;
        }

        [[nodiscard]]
        static int constructions() noexcept {
            return s_constructions;
        }

    private:
        inline static int s_constructions = 0;
    };

    class ThrowingConsumer {
    public:
        explicit ThrowingConsumer(Dependency& dependency) : m_dependency(dependency) {
            ++s_attempts;

            if (s_shouldThrow) {
                throw std::runtime_error("Intentional consumer construction failure");
            }
        }

        static void reset() noexcept {
            s_attempts = 0;
            s_shouldThrow = true;
        }

        static void allowConstruction() noexcept {
            s_shouldThrow = false;
        }

        [[nodiscard]]
        static int attempts() noexcept {
            return s_attempts;
        }

    private:
        Dependency& m_dependency;

        inline static int s_attempts = 0;
        inline static bool s_shouldThrow = true;
    };

    void testFailedConsumerConstructionPreservesDependencyCache() {
        constexpr std::string_view testName =
            "Failed consumer construction preserves dependency cache";

        Dependency::reset();
        ThrowingConsumer::reset();

        di::Container container;

        container.addService<Dependency>(di::ServiceLifetime::Scoped);

        container.addService<ThrowingConsumer, Dependency>(di::ServiceLifetime::Scoped);

        container.finalizeRegistrations();

        auto scope = container.createScope();

        try {
            [[maybe_unused]]
            auto& consumer = scope.resolve<ThrowingConsumer>();

            testing::fail(testName, "First consumer resolution unexpectedly succeeded");

            return;
        } catch (const std::runtime_error&) {
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
            return;
        }

        ThrowingConsumer::allowConstruction();

        try {
            auto& firstSuccessful = scope.resolve<ThrowingConsumer>();

            auto& secondSuccessful = scope.resolve<ThrowingConsumer>();

            if (&firstSuccessful != &secondSuccessful) {
                testing::fail(testName, "Successful consumer was not cached");

                return;
            }

            if (Dependency::constructions() != 1) {
                testing::fail(
                    testName,
                    "Scoped dependency was reconstructed after consumer failure");

                return;
            }

            if (ThrowingConsumer::attempts() != 2) {
                testing::fail(testName, "Consumer construction attempt count was incorrect");

                return;
            }

            testing::success(testName);
        } catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
    }
}

int main() {
    testFailedConsumerConstructionPreservesDependencyCache();

    return 0;
}
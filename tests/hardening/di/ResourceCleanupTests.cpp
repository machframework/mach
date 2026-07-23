#include <string_view>

#include "Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

namespace
{
    namespace di = mach::detail::di;

    class TrackedSingleton {
    public:
        TrackedSingleton() {
            ++s_aliveCount;
        }

        ~TrackedSingleton() {
            --s_aliveCount;
        }

        static void reset() noexcept {
            s_aliveCount = 0;
        }

        [[nodiscard]]
        static int aliveCount() noexcept {
            return s_aliveCount;
        }

    private:
        inline static int s_aliveCount = 0;
    };

    void testSingletonDestroyedWithContainer() {
        constexpr std::string_view testName = "Singleton instance destroyed with container";

        TrackedSingleton::reset();

        {
            di::Container container;

            container.addService<TrackedSingleton>(di::ServiceLifetime::Singleton);

            container.finalizeRegistrations();

            auto scope = container.createScope();

            [[maybe_unused]]
            auto& singleton = scope.resolve<TrackedSingleton>();

            if (TrackedSingleton::aliveCount() != 1) {
                testing::fail(testName, "Singleton was not alive after resolution");

                return;
            }
        }

        if (TrackedSingleton::aliveCount() != 0) {
            testing::fail(testName, "Singleton survived container destruction");

            return;
        }

        testing::success(testName);
    }
}

int main() {
    testSingletonDestroyedWithContainer();

    return 0;
}

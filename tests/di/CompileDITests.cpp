#include <string>

#include <mach/detail/di/Container.hpp>

namespace test_compile_di
{

    struct Logger {};

    struct Repository {
        explicit Repository(Logger& logger) : logger(logger) {}

        Logger& logger;
    };

    struct Service {
        Service(Repository& repository, Logger& logger) : repository(repository), logger(logger) {}

        Repository& repository;
        Logger& logger;
    };

} // namespace test_compile_di

static void runDiRegistrationCompileTests() {
    using namespace test_compile_di;
    namespace di = mach::detail::di;

    //{
    //    const std::string testName = "Valid empty constructor registration compiles";

    //    di::Container container;
    //    container.addService<Logger>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Valid dependency registration compiles";

    //    di::Container container;
    //    container.addService<Logger>(di::ServiceLifetime::Scoped);
    //    container.addService<Repository, Logger>(di::ServiceLifetime::Scoped);
    //    container.addService<Service, Repository, Logger>(di::ServiceLifetime::Transient);
    //}

    //{
    //    const std::string testName = "Invalid primitive service registration fails to compile";

    //    di::Container container;
    //    container.addService<int>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid string service registration fails to compile";

    //    di::Container container;
    //    container.addService<std::string>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid pointer service registration fails to compile";

    //    di::Container container;
    //    container.addService<Logger*>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid reference service registration fails to compile";

    //    di::Container container;
    //    container.addService<Logger&>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid const service registration fails to compile";

    //    di::Container container;
    //    container.addService<const Logger>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid primitive dependency registration fails to compile";

    //    di::Container container;
    //    container.addService<Repository, int>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid string dependency registration fails to compile";

    //    di::Container container;
    //    container.addService<Repository, std::string>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid pointer dependency registration fails to compile";

    //    di::Container container;
    //    container.addService<Repository, Logger*>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid reference dependency registration fails to compile";

    //    di::Container container;
    //    container.addService<Repository, Logger&>(di::ServiceLifetime::Scoped);
    //}

    //{
    //    const std::string testName = "Invalid const dependency registration fails to compile";

    //    di::Container container;
    //    container.addService<Repository, const Logger>(di::ServiceLifetime::Scoped);
    //}
}

int main() {
    runDiRegistrationCompileTests();
}
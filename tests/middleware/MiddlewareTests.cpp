#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/middleware/Next.hpp>

#include "support/BeastHttpClient.hpp"

namespace
{
    class Trace {
    public:
        static void reset() {
            std::lock_guard lock(s_mutex);
            s_value.clear();
        }

        static void append(std::string_view value) {
            std::lock_guard lock(s_mutex);
            s_value += value;
        }

        [[nodiscard]]
        static std::string value() {
            std::lock_guard lock(s_mutex);
            return s_value;
        }

    private:
        inline static std::mutex s_mutex;
        inline static std::string s_value;
    };

    class Dependency {
    public:
        [[nodiscard]]
        std::string_view message() const noexcept {
            return "dependency injected";
        }
    };

    class FirstMiddleware {
    public:
        void invoke(mach::Context& context, mach::Next& next) {
            if (context.request.target() != "/order") {
                next();
                return;
            }

            Trace::append("A>");

            next();

            Trace::append("<A");

            context.response.body(Trace::value());
        }
    };

    class SecondMiddleware {
    public:
        void invoke(mach::Context& context, mach::Next& next) {
            if (context.request.target() != "/order") {
                next();
                return;
            }

            Trace::append("B>");

            next();

            Trace::append("<B");
        }
    };

    class DependencyMiddleware {
    public:
        explicit DependencyMiddleware(Dependency& dependency) : m_dependency(dependency) {}

        void invoke(mach::Context& context, mach::Next& next) {
            next();

            if (context.request.target() == "/dependency") {
                context.response.body(std::string(m_dependency.message()));
            }
        }

    private:
        Dependency& m_dependency;
    };

    class ShortCircuitMiddleware {
    public:
        void invoke(mach::Context& context, mach::Next& next) {
            if (context.request.target() != "/short-circuit") {
                next();
                return;
            }

            context.response.status(mach::http::StatusCode::Unauthorized);

            context.response.body("blocked");
        }
    };

    class DoubleNextMiddleware {
    public:
        void invoke(mach::Context& context, mach::Next& next) {
            if (context.request.target() != "/double-next") {
                next();
                return;
            }

            next();
            next();
        }
    };

    class MiddlewareServer {
    public:
        MiddlewareServer()
            : m_app(
                  mach::AppBuilder()
                      .addScoped<Dependency>()
                      .use<FirstMiddleware>()
                      .use<SecondMiddleware>()
                      .use<DependencyMiddleware, Dependency>()
                      .use<ShortCircuitMiddleware>()
                      .use<DoubleNextMiddleware>()
                      .build()) {
            m_app.mapGet("/order", [](mach::Context&) {
                Trace::append("endpoint");
            });

            m_app.mapGet("/dependency", [](mach::Context&) {
            });

            m_app.mapGet("/short-circuit", [](mach::Context& context) {
                context.response.body("endpoint");
            });

            m_app.mapGet("/double-next", [](mach::Context& context) {
                context.response.body("endpoint");
            });

            m_thread = std::thread([this] {
                m_app.run();
            });

            waitUntilReady();
        }

        ~MiddlewareServer() {
            m_app.stop();

            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        MiddlewareServer(const MiddlewareServer&) = delete;
        MiddlewareServer& operator=(const MiddlewareServer&) = delete;

    private:
        void waitUntilReady() {
            constexpr int attempts = 100;

            testing::http::BeastHttpClient client;

            for (int i = 0; i < attempts; ++i) {
                try {
                    (void)client.get("/dependency");
                    return;
                } catch (...) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            throw std::runtime_error("Mach middleware test server failed to start");
        }

        mach::App m_app;
        std::thread m_thread;
    };

    MiddlewareServer& middlewareServer() {
        static MiddlewareServer server;
        return server;
    }

    testing::http::BeastHttpClient& httpClient() {
        static testing::http::BeastHttpClient client;
        return client;
    }

    void ensureServerRunning() {
        (void)middlewareServer();
    }
}

TEST_CASE("Middleware executes in pipeline order") {
    ensureServerRunning();

    Trace::reset();

    const auto response = httpClient().get("/order");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "A>B>endpoint<B<A");
}

TEST_CASE("Middleware dependencies are resolved through DI") {
    ensureServerRunning();

    const auto response = httpClient().get("/dependency");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "dependency injected");
}

TEST_CASE("Middleware can short-circuit the pipeline") {
    ensureServerRunning();

    const auto response = httpClient().get("/short-circuit");

    REQUIRE(response.statusCode() == 401);
    REQUIRE(response.body() == "blocked");
}

TEST_CASE("Middleware cannot invoke next more than once") {
    ensureServerRunning();

    const auto response = httpClient().get("/double-next");

    REQUIRE(response.statusCode() == 500);
}

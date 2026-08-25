#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/mach.hpp>

#include "support/BeastHttpClient.hpp"

namespace
{
    using testing::http::BeastHttpClient;

    struct TestRequest {
        std::string name;
        int age;

        void validate(mach::ValidationBuilder<TestRequest>& builder) const {
            builder.field(&TestRequest::name).regex(R"(^[A-Za-z]+$)").length(1, 10);

            builder.field(&TestRequest::age).range(1, 5);
        }
    };

    MACH_DEFINE_JSON(TestRequest, name, age);

    class ValidationServer {
    public:
        ValidationServer() : m_app(mach::AppBuilder().build()) {
            m_app.mapPost("/test", [](TestRequest request) {
                return mach::ok(request);
            });

            m_thread = std::thread([this] {
                m_app.run();
            });

            waitUntilReady();
        }

        ~ValidationServer() {
            m_app.stop();

            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        ValidationServer(const ValidationServer&) = delete;
        ValidationServer& operator=(const ValidationServer&) = delete;

    private:
        void waitUntilReady() {
            constexpr int attempts = 100;

            BeastHttpClient client;

            for (int i = 0; i < attempts; ++i) {
                try {
                    (void)client.post(
                        "/test",
                        R"({"name":"Asaf","age":3})",
                        {{"Content-Type", "application/json"}});

                    return;
                } catch (...) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            throw std::runtime_error("Mach validation test server failed to start");
        }

        mach::App m_app;
        std::thread m_thread;
    };

    ValidationServer& validationServer() {
        static ValidationServer server;
        return server;
    }

    BeastHttpClient& httpClient() {
        static BeastHttpClient client;
        return client;
    }

    void ensureServerRunning() {
        (void)validationServer();
    }

    BeastHttpClient::Headers jsonHeaders() {
        return {{"Content-Type", "application/json"}};
    }
}

TEST_CASE("Validation accepts valid request bodies") {
    ensureServerRunning();

    const auto response = httpClient().post("/test", R"({"name":"Asaf","age":3})", jsonHeaders());

    REQUIRE(response.statusCode() == 200);
}

TEST_CASE("Validation rejects invalid string fields") {
    ensureServerRunning();

    SECTION("Regex") {
        const auto response =
            httpClient().post("/test", R"({"name":"Asaf123","age":3})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Minimum length") {
        const auto response = httpClient().post("/test", R"({"name":"","age":3})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Maximum length") {
        const auto response =
            httpClient().post("/test", R"({"name":"ABCDEFGHIJK","age":3})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }
}

TEST_CASE("Validation rejects values outside numeric range") {
    ensureServerRunning();

    SECTION("Below minimum") {
        const auto response =
            httpClient().post("/test", R"({"name":"Asaf","age":0})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Above maximum") {
        const auto response =
            httpClient().post("/test", R"({"name":"Asaf","age":6})", jsonHeaders());

        REQUIRE(response.statusCode() == 400);
    }

    SECTION("Boundary values are accepted") {
        const auto minimum =
            httpClient().post("/test", R"({"name":"Asaf","age":1})", jsonHeaders());

        const auto maximum =
            httpClient().post("/test", R"({"name":"Asaf","age":5})", jsonHeaders());

        REQUIRE(minimum.statusCode() == 200);
        REQUIRE(maximum.statusCode() == 200);
    }
}

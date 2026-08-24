#include <catch2/catch_test_macros.hpp>

#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>

#include "support/BeastHttpClient.hpp"

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

namespace
{

    class RoutingServer {
    public:
        RoutingServer() : app_(mach::AppBuilder().build()) {
            registerRoutes();

            thread_ = std::thread([this] {
                app_.run();
            });

            waitUntilReady();
        }

        ~RoutingServer() {
            app_.stop();

            if (thread_.joinable()) {
                thread_.join();
            }
        }

        RoutingServer(const RoutingServer&) = delete;
        RoutingServer& operator=(const RoutingServer&) = delete;

    private:
        void registerRoutes() {
            app_.mapGet("/", [](mach::Context& context) {
                context.response.body("root");
            });

            app_.mapGet("/static", [](mach::Context& context) {
                context.response.body("static");
            });

            app_.mapGet("/users/{id}", [](mach::Context& context) {
                context.response.body(std::string(context.request.routeParam("id")));
            });

            app_.mapGet("/users/{userId}/posts/{postId}", [](mach::Context& context) {
                context.response.body(
                    std::string(context.request.routeParam("userId")) + ":" +
                    std::string(context.request.routeParam("postId")));
            });

            app_.mapGet("/precedence/{id}", [](mach::Context& context) {
                context.response.body("parameter");
            });

            app_.mapGet("/precedence/me", [](mach::Context& context) {
                context.response.body("static");
            });

            app_.mapGet("/orders/{id:int}", [](mach::Context& context) {
                context.response.body(std::string(context.request.routeParam("id")));
            });

            app_.mapGet("/get-only", [](mach::Context& context) {
                context.response.body("GET");
            });

            app_.mapPost("/post-only", [](mach::Context& context) {
                context.response.body("POST");
            });

            app_.mapGet("/me/profile", [](mach::Context& context) {
                context.response.body("static");
            });

            app_.mapGet("/{username}/posts", [](mach::Context& context) {
                context.response.body(std::string(context.request.routeParam("username")));
            });

            app_.mapGet("/me/profile/details", [](mach::Context& context) {
                context.response.body("static");
            });

            app_.mapGet("/{username}/posts/archive/details", [](mach::Context& context) {
                context.response.body("archive");
            });

            app_.mapGet("/{username}/posts/{postId}/comments", [](mach::Context& context) {
                context.response.body(
                    std::string(context.request.routeParam("username")) + ":" +
                    std::string(context.request.routeParam("postId")));
            });
        }

        void waitUntilReady() {
            constexpr int attempts = 100;

            testing::http::BeastHttpClient client;

            for (int i = 0; i < attempts; ++i) {
                try {
                    (void)client.get("/");
                    return;
                } catch (...) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            throw std::runtime_error("Mach test server failed to start");
        }

        mach::App app_;
        std::thread thread_;
    };

    RoutingServer& routingServer() {
        static RoutingServer server;
        return server;
    }

    void ensureServerRunning() {
        (void)routingServer();
    }

    testing::http::BeastHttpClient& httpClient() {
        static testing::http::BeastHttpClient client;
        return client;
    }

} // namespace

// -----------------------------------------------------------------------------
// Registration
// -----------------------------------------------------------------------------

TEST_CASE("Valid route patterns can be registered") {
    auto app = mach::AppBuilder().build();

    SECTION("Root route") {
        REQUIRE_NOTHROW(app.mapGet("/", [](mach::Context&) {
        }));
    }

    SECTION("Static route") {
        REQUIRE_NOTHROW(app.mapGet("/users", [](mach::Context&) {
        }));
    }

    SECTION("Parameter route") {
        REQUIRE_NOTHROW(app.mapGet("/users/{id}", [](mach::Context&) {
        }));
    }

    SECTION("Explicit string constraint") {
        REQUIRE_NOTHROW(app.mapGet("/users/{name:string}", [](mach::Context&) {
        }));
    }

    SECTION("Integer constraint") {
        REQUIRE_NOTHROW(app.mapGet("/users/{id:int}", [](mach::Context&) {
        }));
    }

    SECTION("Trailing slash") {
        REQUIRE_NOTHROW(app.mapGet("/users/", [](mach::Context&) {
        }));
    }
}

TEST_CASE("Compatible routes can coexist") {
    auto app = mach::AppBuilder().build();

    SECTION("Same path with different methods") {
        app.mapGet("/users", [](mach::Context&) {
        });

        REQUIRE_NOTHROW(app.mapPost("/users", [](mach::Context&) {
        }));
    }

    SECTION("Static and parameter routes") {
        app.mapGet("/users/{id}", [](mach::Context&) {
        });

        REQUIRE_NOTHROW(app.mapGet("/users/me", [](mach::Context&) {
        }));
    }

    SECTION("Parent and child parameter routes") {
        app.mapGet("/users/{id}/profile", [](mach::Context&) {
        });

        REQUIRE_NOTHROW(app.mapGet("/users/{id}", [](mach::Context&) {
        }));
    }

    SECTION("Different parameter constraints") {
        app.mapGet("/users/{value}", [](mach::Context&) {
        });

        REQUIRE_NOTHROW(app.mapGet("/users/{value:int}", [](mach::Context&) {
        }));
    }
}

TEST_CASE("Conflicting route registrations are rejected") {
    auto app = mach::AppBuilder().build();

    SECTION("Exact duplicate") {
        app.mapGet("/users", [](mach::Context&) {
        });

        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users",
                [](mach::Context&) {
                }),
            std::logic_error);
    }

    SECTION("Equivalent parameter shape") {
        app.mapGet("/users/{id}", [](mach::Context&) {
        });

        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{name}",
                [](mach::Context&) {
                }),
            std::logic_error);
    }
}

TEST_CASE("Malformed route syntax is rejected") {
    auto app = mach::AppBuilder().build();

    SECTION("Empty route") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Missing leading slash") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "users",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Query marker") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users?active=true",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Fragment marker") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users#active",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Repeated slash") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users//profile",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Empty parameter") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Unclosed parameter") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Unopened parameter") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/id}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Extra closing brace") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id}}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Nested parameter") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{{id}}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Mixed nested braces") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id:{int}}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Adjacent empty parameter") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id}{}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Static prefix") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/user-{id}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Static suffix") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id}.json",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Text after parameter") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id}name",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Whitespace in parameter name") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{user id}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Duplicate parameter names") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id}/posts/{id}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Duplicate parameter names regardless of constraint") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id:int}/posts/{id}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }
}

TEST_CASE("Invalid route constraints are rejected") {
    auto app = mach::AppBuilder().build();

    SECTION("Empty parameter name") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{:int}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Empty constraint") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id:}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Unknown constraint") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id:banana}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Too many colons") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id:int:extra}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Whitespace in constraint") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{id: int}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }

    SECTION("Constraint contains invalid character") {
        REQUIRE_THROWS_AS(
            app.mapGet(
                "/users/{name:i#nt}",
                [](mach::Context&) {
                }),
            std::invalid_argument);
    }
}

// -----------------------------------------------------------------------------
// Runtime matching
// -----------------------------------------------------------------------------

TEST_CASE("Root and static routes are matched") {
    ensureServerRunning();

    SECTION("Root route") {
        const auto response = httpClient().get("/");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "root");
    }

    SECTION("Static route") {
        const auto response = httpClient().get("/static");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "static");
    }
}

TEST_CASE("Route parameters are extracted") {
    ensureServerRunning();

    SECTION("Single parameter") {
        const auto response = httpClient().get("/users/123");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "123");
    }

    SECTION("Multiple parameters") {
        const auto response = httpClient().get("/users/asaf/posts/42");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "asaf:42");
    }
}

TEST_CASE("Static routes take precedence over parameter routes") {
    ensureServerRunning();

    const auto response = httpClient().get("/precedence/me");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "static");
}

TEST_CASE("Integer constraints match valid integers") {
    ensureServerRunning();

    SECTION("Positive integer") {
        const auto response = httpClient().get("/orders/123");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "123");
    }

    SECTION("Negative integer") {
        const auto response = httpClient().get("/orders/-123");

        REQUIRE(response.statusCode() == 200);
        REQUIRE(response.body() == "-123");
    }
}

TEST_CASE("Integer constraints reject invalid values") {
    ensureServerRunning();

    SECTION("Non-numeric value") {
        const auto response = httpClient().get("/orders/abc");

        REQUIRE(response.statusCode() == 404);
    }

    SECTION("Partially numeric value") {
        const auto response = httpClient().get("/orders/12abc");

        REQUIRE(response.statusCode() == 404);
    }

    SECTION("Explicit positive sign") {
        const auto response = httpClient().get("/orders/+123");

        REQUIRE(response.statusCode() == 404);
    }
}

TEST_CASE("Unknown routes return Not Found") {
    ensureServerRunning();

    const auto response = httpClient().get("/does-not-exist");

    REQUIRE(response.statusCode() == 404);
}

TEST_CASE("Unsupported methods return Method Not Allowed") {
    ensureServerRunning();

    const auto response = httpClient().post("/get-only");

    REQUIRE(response.statusCode() == 405);
}

TEST_CASE("Routes are matched by HTTP method") {
    ensureServerRunning();

    const auto response = httpClient().post("/post-only");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "POST");
}

TEST_CASE("Routing falls back from a failed static branch") {
    ensureServerRunning();

    const auto response = httpClient().get("/me/posts");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "me");
}

TEST_CASE("Routing can backtrack across multiple branches") {
    ensureServerRunning();

    const auto response = httpClient().get("/me/posts/archive/comments");

    REQUIRE(response.statusCode() == 200);
    REQUIRE(response.body() == "me:archive");
}

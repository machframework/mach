#include <string>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/Json.hpp>
#include <mach/results/Results.hpp>

#include "Testing.hpp"

struct CreateUserRequest {
    std::string name;
    int age;
};

MACH_DEFINE_JSON(CreateUserRequest, name, age);

struct UpdateScoreRequest {
    int score;
};

MACH_DEFINE_JSON(UpdateScoreRequest, score);

void registerContentTypeTestingEndpoints(mach::App& app)
{
    // No body parameter:
    // Content-Type should not be required or validated.
    app.mapGet(
        "/testing/content-type/no-body",
        []() {
            return mach::ok(
                "Request succeeded without body binding."
            );
        }
    );

    // DTO body binding.
    app.mapPost(
        "/testing/content-type/user",
        [](CreateUserRequest request) {
            return mach::ok(
                "Created user '" +
                request.name +
                "' with age " +
                std::to_string(request.age) +
                "."
            );
        }
    );

    // Primitive JSON body binding.
    app.mapPost(
        "/testing/content-type/integer",
        [](int value) {
            return mach::ok(
                "Received integer: " + std::to_string(value)
            );
        }
    );

    // JSON string body binding.
    //
    // The body must be a JSON string:
    //     "hello"
    //
    // It must not be plain text:
    //     hello
    app.mapPost(
        "/testing/content-type/string",
        [](std::string value) {
            return mach::ok(
                "Received string: " + value
            );
        }
    );

    // Context + DTO body binding.
    app.mapPut(
        "/testing/content-type/score",
        [](mach::Context& context, UpdateScoreRequest request) {
            context.response.setHeader(
                "X-Testing-Endpoint",
                "content-type"
            );

            return mach::ok(
                "Received score: " + std::to_string(request.score)
            );
        }
    );
}

int main()
{
    mach::AppBuilder builder(std::move(testing::serverOptions));

    auto app = builder.build();

    registerContentTypeTestingEndpoints(app);

    return app.run();
}
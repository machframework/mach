#include <iostream>
#include <string>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/Json.hpp>
#include <mach/results/Results.hpp>

#include "Testing.hpp"

struct CreateUserRequest
{
    std::string name;
    int age;
};

MACH_DEFINE_JSON(CreateUserRequest, name, age);

struct UpdateScoreRequest
{
    int score;
};

MACH_DEFINE_JSON(UpdateScoreRequest, score);

void registerContentTypeTestingEndpoints(mach::App& app)
{
    // Content-Type must not be required when no body binding occurs.
    app.mapGet(
        "/testing/content-type/no-body",
        []() {
            return mach::ok(
                "Request succeeded without body binding."
            );
        }
    );

    // DTO JSON body binding.
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
                "Received integer: " +
                std::to_string(value)
            );
        }
    );

    // JSON string body binding.
    //
    // Valid JSON body:
    //     "hello"
    //
    // Invalid JSON body:
    //     hello
    app.mapPost(
        "/testing/content-type/string",
        [](std::string value) {
            return mach::ok(
                "Received string: " + value
            );
        }
    );

    // Context + DTO JSON body binding.
    app.mapPut(
        "/testing/content-type/score",
        [](mach::Context& context, UpdateScoreRequest request) {
            context.response.setHeader(
                "X-Testing-Endpoint",
                "content-type"
            );

            return mach::ok(
                "Received score: " +
                std::to_string(request.score)
            );
        }
    );
}

void printTestingInstructions()
{
    std::cout << R"(

    ============================================================
    Mach Content-Type Binding Tests
    ============================================================

    These commands are intended for Windows PowerShell.

    1. No body parameter and no Content-Type
       Expected: 200 OK

    curl.exe --% -i http://127.0.0.1:3143/testing/content-type/no-body


    2. No body parameter with an unrelated Content-Type
       Expected: 200 OK

    curl.exe --% -i -H "Content-Type: text/plain" http://127.0.0.1:3143/testing/content-type/no-body


    3. DTO with application/json
       Expected: 200 OK

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/content-type/user


    4. DTO with application/json and charset
       Expected: 200 OK

    curl.exe --% -i -X POST -H "Content-Type: application/json; charset=utf-8" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/content-type/user


    5. DTO with a structured JSON media type
       Expected: 415 Unsupported Media Type

    curl.exe --% -i -X POST -H "Content-Type: application/vnd.mach+json" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/content-type/user


    6. DTO with no Content-Type
       Expected: 415 Unsupported Media Type

    curl.exe --% -i -X POST -H "Content-Type:" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/content-type/user


    7. DTO with text/plain
       Expected: 415 Unsupported Media Type

    curl.exe --% -i -X POST -H "Content-Type: text/plain" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/content-type/user


    8. DTO with application/xml
       Expected: 415 Unsupported Media Type

    curl.exe --% -i -X POST -H "Content-Type: application/xml" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/content-type/user


    9. Malformed JSON with application/json
       Expected: 400 Bad Request

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\",\"age\":18" http://127.0.0.1:3143/testing/content-type/user


    10. Valid JSON with an incorrect field type
        Expected: 400 Bad Request

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\",\"age\":\"eighteen\"}" http://127.0.0.1:3143/testing/content-type/user


    11. Valid JSON with a missing required field
        Expected: 400 Bad Request

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\"}" http://127.0.0.1:3143/testing/content-type/user


    12. Primitive integer JSON body
        Expected: 200 OK

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary 42 http://127.0.0.1:3143/testing/content-type/integer


    13. Primitive integer with text/plain
        Expected: 415 Unsupported Media Type

    curl.exe --% -i -X POST -H "Content-Type: text/plain" --data-binary 42 http://127.0.0.1:3143/testing/content-type/integer


    14. Proper JSON string body
        Expected: 200 OK

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "\"hello\"" http://127.0.0.1:3143/testing/content-type/string


    15. Unquoted string with application/json
        Expected: 400 Bad Request

    curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary hello http://127.0.0.1:3143/testing/content-type/string


    16. Raw string with text/plain
        Expected: 415 Unsupported Media Type

    curl.exe --% -i -X POST -H "Content-Type: text/plain" --data-binary hello http://127.0.0.1:3143/testing/content-type/string


    17. Context + DTO body parameter
        Expected:
          - 200 OK
          - X-Testing-Endpoint: content-type

    curl.exe --% -i -X PUT -H "Content-Type: application/json" --data-binary "{\"score\":100}" http://127.0.0.1:3143/testing/content-type/score


    18. Context + DTO with unsupported Content-Type
        Expected: 415 Unsupported Media Type

    curl.exe --% -i -X PUT -H "Content-Type: text/plain" --data-binary "{\"score\":100}" http://127.0.0.1:3143/testing/content-type/score

    ============================================================

    )";
}

int main()
{
    mach::AppBuilder builder(std::move(testing::serverOptions));

    auto app = builder.build();

    registerContentTypeTestingEndpoints(app);
    printTestingInstructions();

    return app.run();
}

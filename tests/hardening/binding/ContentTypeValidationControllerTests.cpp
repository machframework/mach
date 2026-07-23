#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/Reply.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

#include "Testing.hpp"

struct CreateControllerUserRequest
{
    std::string name;
    int age;
};

MACH_DEFINE_JSON(CreateControllerUserRequest, name, age);

class ContentTypeController : public mach::ControllerBase
{
public:
    inline static constexpr std::string_view route =
        "/testing/controllers/content-type";

    static void configure(
        mach::ControllerBuilder<ContentTypeController>& builder
    )
    {
        builder.mapGet(
            "/no-body",
            &ContentTypeController::noBody
        );

        builder.mapPost(
            "/user",
            &ContentTypeController::createUser
        );
    }

    mach::Reply<std::string> noBody()
    {
        return ok(
            "Controller request succeeded without body binding."
        );
    }

    mach::Reply<std::string> createUser(
        CreateControllerUserRequest request
    )
    {
        response().setHeader(
            "X-Testing-Controller",
            "content-type"
        );

        return ok(
            "Created controller user '" +
            request.name +
            "' with age " +
            std::to_string(request.age) +
            "."
        );
    }
};

void printTestingInstructions()
{
    std::cout << R"(

============================================================
Mach Controller Content-Type Binding Tests
============================================================

These commands are intended for Windows PowerShell.

Replace 3143 with the configured testing server port.

1. Controller action with no body parameter and no Content-Type
   Expected: 200 OK

curl.exe --% -i http://127.0.0.1:3143/testing/controllers/content-type/no-body


2. Controller DTO action with application/json
   Expected:
     - 200 OK
     - x-testing-controller: content-type

curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/controllers/content-type/user


3. Controller DTO action with no Content-Type
   Expected: 415 Unsupported Media Type

curl.exe --% -i -X POST -H "Content-Type:" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/controllers/content-type/user


4. Controller DTO action with text/plain
   Expected: 415 Unsupported Media Type

curl.exe --% -i -X POST -H "Content-Type: text/plain" --data-binary "{\"name\":\"asaf\",\"age\":18}" http://127.0.0.1:3143/testing/controllers/content-type/user


5. Controller DTO action with malformed JSON
   Expected: 400 Bad Request
   Expected body: The request body contains invalid JSON.

curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\",\"age\":18" http://127.0.0.1:3143/testing/controllers/content-type/user


6. Controller DTO action with an incorrect field type
   Expected: 400 Bad Request
   Expected body: One or more JSON fields have an invalid type.

curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\",\"age\":\"eighteen\"}" http://127.0.0.1:3143/testing/controllers/content-type/user


7. Controller DTO action with a missing required field
   Expected: 400 Bad Request
   Expected body: A required JSON field is missing.

curl.exe --% -i -X POST -H "Content-Type: application/json" --data-binary "{\"name\":\"asaf\"}" http://127.0.0.1:3143/testing/controllers/content-type/user

============================================================

)";
}

int main()
{
    mach::AppBuilder builder(std::move(testing::serverOptions));

    builder.addController<ContentTypeController>();

    auto app = builder.build();

    printTestingInstructions();

    return app.run();
}

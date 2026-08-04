#include <mach/diagnostics/TerminateHandler.hpp>
#include <mach/mach.hpp>

#include "Testing.hpp"

struct TestRequest {
    std::string name;
    int age;

    void validate(mach::ValidationBuilder<TestRequest>& builder) const {
        auto f = builder.field(&TestRequest::age);
        builder.field(&TestRequest::name).regex(R"(^[A-Za-z]+$)").length(1, 10);

        builder.field(&TestRequest::age).range(1, 5);
    }
};

MACH_DEFINE_JSON(TestRequest, name, age)

int main() {
    mach::installTerminateHandler();

    mach::App app = mach::AppBuilder{}.build();

    app.mapPost("/test", [](TestRequest request) {
        return mach::ok(request);
    });

    return app.run();
}

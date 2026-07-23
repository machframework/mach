#include <string_view>

#include <mach/AppBuilder.hpp>
#include <mach/Reply.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/diagnostics/TerminateHandler.hpp>

#include "Testing.hpp"

class GeneralController : public mach::ControllerBase {

public:
    inline static constexpr std::string_view route = "/general";

    mach::Reply<int> getAge() {
        return ok(5);
    }

    mach::Reply<int> getAge2() {
        return ok(5);
    }

    static void configure(mach::ControllerBuilder<GeneralController>& routes) {
        routes.mapGet("/age", &GeneralController::getAge);
        routes.mapGet(&GeneralController::getAge2);
    }
};

int main() {
    mach::installTerminateHandler();

    auto builder = mach::AppBuilder(std::move(testing::serverOptions));
    builder.addController<GeneralController>();

    auto app = builder.build();
    return app.run();
}

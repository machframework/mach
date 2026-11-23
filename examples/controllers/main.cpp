#include <optional>
#include <string>
#include <string_view>

#include <mach/controllers.hpp>

class HomeController : public mach::ControllerBase {

public:
    HomeController(mach::Logger& logger) : m_logger(logger) {}

    inline static constexpr std::string_view route = "/home";

    mach::Reply<int> doubleNumber(mach::Param<int, "number"> number) {
        return ok(*number * 2);
    }

    mach::Reply<std::string> sayHi() {
        m_logger.info("Handling GET /home/hi");
        return ok("Hi from Mach!");
    }

    mach::Reply<std::string> greet(
        mach::Param<std::string, "name"> name,
        mach::Query<std::optional<int>, "times"> times) {
        std::string message;

        for (int i = 0; i < times->value_or(1); ++i) {
            if (!message.empty()) {
                message += " ";
            }

            message += "Hello, " + *name + "!";
        }

        return ok(message);
    }

    static void configure(mach::ControllerBuilder<HomeController>& routes) {
        routes.mapGet("/{number:int}", &HomeController::doubleNumber);
        routes.mapGet("/hi", &HomeController::sayHi);
        routes.mapGet("/greet/{name}", &HomeController::greet);
    }

private:
    mach::Logger& m_logger;
};

int main() {
    mach::AppBuilder builder;

    builder.addController<HomeController, mach::Logger>();

    auto app = builder.build();
    return app.run();
}

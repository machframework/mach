#include <string>
#include <string_view>

#include <mach/controllers.hpp>

class HomeController : public mach::ControllerBase {
public:
    HomeController(mach::Logger& logger) : m_logger(logger) {}

    inline static constexpr std::string_view route = "/home";

    mach::Reply<int> doubleNumber() {
        const int number = request().routeParam<int>("number");
        return ok(number * 2);
    }

    mach::Reply<std::string> sayHi() {
        m_logger.info("Handling GET /home/hi");
        return ok("Hi from Mach!");
    }

    static void configure(mach::ControllerBuilder<HomeController>& routes) {
        routes.mapGet("/{number:int}", &HomeController::doubleNumber);
        routes.mapGet("/hi", &HomeController::sayHi);
    }

private:
    mach::Logger& m_logger;
};

int main() {
    auto builder = mach::AppBuilder();

    builder.addController<HomeController, mach::Logger>();

    auto app = builder.build();
    return app.run();
}

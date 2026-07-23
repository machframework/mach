#include <exception>
#include <iostream>
#include <string_view>
#include <thread>

#include <mach/controllers.hpp>

class HomeController : public mach::ControllerBase {

public:
    inline static constexpr std::string_view route = "/home";

    mach::Reply<int> calculateAge() {
        constexpr int currentYear = 2026;

        const int birthYear = request().routeParam<int>("birth");
        const int age = currentYear - birthYear;

        return ok(age);
    }

    mach::Reply<std::string> sayHi() {
        return ok("Hi");
    }

    static void configure(mach::ControllerBuilder<HomeController>& routes) {
        routes.mapGet("/{birth:int}", &HomeController::calculateAge);
        routes.mapGet("/hi", &HomeController::sayHi);
    }
};

int main() {
    const auto hardwareThreads = std::thread::hardware_concurrency();
    auto builder = mach::AppBuilder(
        mach::ServerOptions{
            .host = "127.0.0.1",
            .port = 3143,
            .threadCount = std::thread::hardware_concurrency()});

    builder.addController<HomeController>();
    auto app = builder.build();

    return app.run();
}

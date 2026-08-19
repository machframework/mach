#include <string>
#include <string_view>

#include <mach/controllers.hpp>

class GreetingService {
public:
    std::string greet(std::string_view name) const {
        return "Hello, " + std::string(name) + "!";
    }
};

class GreetingController : public mach::ControllerBase {
public:
    inline static constexpr std::string_view route = "/greetings";

    explicit GreetingController(GreetingService& greetings) : m_greetings(greetings) {}

    mach::Reply<std::string> greet() {
        const auto name = request().routeParam("name");
        return ok(m_greetings.greet(name));
    }

    static void configure(mach::ControllerBuilder<GreetingController>& routes) {
        routes.mapGet("/{name}", &GreetingController::greet);
    }

private:
    GreetingService& m_greetings;
};

int main() {
    auto builder = mach::AppBuilder();

    builder.addSingleton<GreetingService>();
    builder.addController<GreetingController, GreetingService>();

    auto app = builder.build();
    return app.run();
}
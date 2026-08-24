#include <iostream>

#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/middleware/Next.hpp>

#include "support/Testing.hpp"

class A {
public:
    void invoke(mach::Context& context, mach::Next& next) {
        std::cout << "Before A\n";
        next();
        next();
        std::cout << "After A\n";
    }
};

class B {
public:
    void invoke(mach::Context& context, mach::Next& next) {
        std::cout << "Before B\n";
        context.response.status(mach::http::StatusCode::Unauthorized);
        std::cout << "After B\n";
    }
};

class C {
public:
    void invoke(mach::Context& context, mach::Next& next) {
        std::cout << "Before C\n";
        next();
        std::cout << "After C\n";
    }
};

int main() {
    auto builder = mach::AppBuilder();

    builder.use<A>().use<B>().use<C>();

    auto app = builder.build();

    app.mapGet("/", [](mach::Context& context) {
        context.response.setHeader("X-Middleware-Tests", "1");
    });

    return app.run();
}

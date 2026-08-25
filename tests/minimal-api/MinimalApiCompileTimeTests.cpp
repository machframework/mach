#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>

namespace
{
    void doNothing(mach::Context& context) {
        context.response.setHeader("X-Arrived", "1");
    }

    void instantiateValidHandlers() {
        mach::AppBuilder builder;
        auto app = builder.build();

        int count = 0;

        app.mapGet("/noexcept", [count]() mutable noexcept {
            ++count;
        });

        app.mapGet("/free-function", doNothing);

        app.mapGet("/function-pointer", &doNothing);
    }
}

int main() {
    instantiateValidHandlers();

    return 0;
}

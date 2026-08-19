#include <string>

#include <mach/mach.hpp>

int main() {
    auto builder = mach::AppBuilder();
    auto app = builder.build();

    app.mapGet("/search", [](mach::Context& context) {
        const auto query = context.request.query("q");
        const auto page = context.request.query<int>("page");

        if (!query) {
            return mach::badRequest<std::string>();
        }

        const int pageNumber = page.value_or(1);

        return mach::ok(
            "Searching for \"" + std::string(*query) + "\" on page " + std::to_string(pageNumber));
    });

    return app.run();
}

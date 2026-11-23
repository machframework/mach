#include <optional>
#include <string>

#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapGet(
        "/search",
        [](mach::Query<std::string, "q"> query, mach::Query<std::optional<int>, "page"> page) {
            const int pageNumber = page->value_or(1);

            return mach::ok(
                "Searching for \"" + *query + "\" on page " + std::to_string(pageNumber));
        });

    return app.run();
}

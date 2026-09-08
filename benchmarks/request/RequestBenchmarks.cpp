#include <cstdint>
#include <string>
#include <thread>
#include <vector>

#include <mach/mach.hpp>

struct SmallRequest {
    std::string name;
    std::string email;
    std::int32_t age;
};

MACH_DEFINE_JSON(SmallRequest, name, email, age)

struct Item {
    std::int64_t id;
    std::string name;
    double price;
    std::int32_t quantity;
};

MACH_DEFINE_JSON(Item, id, name, price, quantity)

struct LargeRequest {
    std::string batchName;
    std::vector<Item> items;
};

MACH_DEFINE_JSON(LargeRequest, batchName, items)

int main() {
    mach::AppBuilder builder;

    builder.configureApp([](mach::AppOptions& options) {
        options.host = "127.0.0.1";
        options.port = 3143;
        options.threadCount = std::thread::hardware_concurrency();
    });

    auto app = builder.build();

    /*
     * Plain request:
     * Measures the normal HTTP -> Mach request pipeline with essentially
     * no request-data access in the handler.
     *
     * This is the main control case for detecting regressions introduced
     * by changes to Request storage, ownership or lifetime.
     */
    app.mapGet("/benchmark/request/plain", [] {
        return mach::noContent();
    });

    /*
     * Headers:
     * Measures inbound header adaptation/storage and header lookup.
     *
     * The client should send a realistic collection of headers including
     * X-Test-1, X-Test-2 and User-Agent.
     */
    app.mapGet("/benchmark/request/headers", [](mach::Context& ctx) {
        const auto test1 = ctx.request.header("X-Test-1");
        const auto test2 = ctx.request.header("X-Test-2");
        const auto userAgent = ctx.request.header("User-Agent");

        if (!test1 || !test2 || !userAgent) {
            return mach::badRequest<void>();
        }

        return mach::noContent();
    });

    /*
     * Cookies:
     * Measures Cookie-header parsing, cookie storage and cookie lookup.
     *
     * The client should send several cookies including session and theme.
     */
    app.mapGet("/benchmark/request/cookies", [](mach::Context& ctx) {
        const auto session = ctx.request.cookie("session");
        const auto theme = ctx.request.cookie("theme");

        if (!session || !theme) {
            return mach::badRequest<void>();
        }

        return mach::noContent();
    });

    /*
     * Raw body:
     * Measures request-body adaptation/storage without JSON binding.
     *
     * Different payload sizes should be supplied by the benchmark client
     * to this same route.
     */
    app.mapPost("/benchmark/request/body", [](mach::Context& ctx) {
        if (ctx.request.body().empty()) {
            return mach::badRequest<void>();
        }

        return mach::noContent();
    });

    /*
     * Small JSON binding:
     * Measures a typical JSON request flowing through the complete request
     * pipeline and then being deserialized into a small DTO.
     */
    app.mapPost("/benchmark/request/binding/small", [](SmallRequest request) {
        const auto valid = !request.name.empty() && !request.email.empty() && request.age >= 0;

        if (!valid) {
            return mach::badRequest<void>();
        }

        return mach::noContent();
    });

    /*
     * Large JSON binding:
     * Measures a larger JSON body containing nested objects and an array.
     *
     * This is especially useful for determining whether eliminating request
     * body copies still produces a measurable end-to-end improvement once
     * JSON parsing, allocation and DTO construction are involved.
     */
    app.mapPost("/benchmark/request/binding/large", [](LargeRequest request) {
        std::size_t totalQuantity = 0;

        for (const auto& item : request.items) {
            totalQuantity += static_cast<std::size_t>(item.quantity);
        }

        if (request.batchName.empty() || totalQuantity == 0) {
            return mach::badRequest<void>();
        }

        return mach::noContent();
    });

    return app.run();
}

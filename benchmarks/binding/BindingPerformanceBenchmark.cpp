#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Json.hpp>
#include <mach/results/Results.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

struct SmallBindingRequest
{
    std::string name;
    std::string email;
    std::int32_t age;
};

MACH_DEFINE_JSON(
    SmallBindingRequest,
    name,
    email,
    age
)

struct BindingItem
{
    std::int64_t id;
    std::string name;
    double price;
    std::int32_t quantity;
};

MACH_DEFINE_JSON(
    BindingItem,
    id,
    name,
    price,
    quantity
)

struct LargeBindingRequest
{
    std::string batchName;
    std::vector<BindingItem> items;
};

MACH_DEFINE_JSON(
    LargeBindingRequest,
    batchName,
    items
)

int main()
{
    auto app = mach::AppBuilder("127.0.0.1", 3143, 16)
        .build();

    /*
     * Baseline route:
     * Measures the HTTP pipeline without body binding.
     */
    app.mapPost(
        "/benchmark/baseline",
        []()
        {
            return mach::noContent();
        }
    );

    /*
     * Small DTO route:
     * Measures typical JSON object deserialization.
     */
    app.mapPost(
        "/benchmark/binding/small",
        [](SmallBindingRequest request)
        {
            // Consume the values so the optimizer cannot trivially discard
            // the bound object before the handler is invoked.
            const auto valid =
                !request.name.empty() &&
                !request.email.empty() &&
                request.age >= 0;

            if (!valid) {
                return mach::badRequest<void>();
            }

            return mach::noContent();
        }
    );

    /*
     * Larger DTO route:
     * Measures JSON array/object deserialization and allocation costs.
     */
    app.mapPost(
        "/benchmark/binding/large",
        [](LargeBindingRequest request)
        {
            std::size_t totalQuantity = 0;

            for (const auto& item : request.items) {
                totalQuantity += static_cast<std::size_t>(item.quantity);
            }

            if (request.batchName.empty() || totalQuantity == 0) {
                return mach::badRequest<void>();
            }

            return mach::noContent();
        }
    );

    /*
     * Failure route:
     * Send malformed JSON or an incompatible DTO to measure the error path.
     * It intentionally uses the same signature as the small binding route.
     */
    app.mapPost(
        "/benchmark/binding/failure",
        [](SmallBindingRequest)
        {
            return mach::noContent();
        }
    );

    return app.run();
}

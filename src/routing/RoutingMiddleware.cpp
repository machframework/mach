#include <mach/detail/routing/RoutingMiddleware.hpp>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::routing
{
    void RoutingMiddleware::invoke(mach::Context& context, mach::Next& next) {
        auto plan = m_router.route(context.request);

        if (!plan.found()) {
            auto statusCode = toStatusCode(plan.status);

            // OPTIONS with no CORS should return 204
            if (context.request.method() == mach::http::Method::Options) {
                statusCode = StatusCode::NoContent;
            }

            context.response.status(statusCode);
            context.response.body(std::string(mach::http::reasonPhrase(statusCode)));

            std::string allow;

            if (plan.allowedMethods.contains(mach::http::Method::Get)) {
                allow += "HEAD";
            }

            for (const mach::http::Method method : mach::http::allMethods) {
                if (!plan.allowedMethods.contains(method)) {
                    continue;
                }
                if (!allow.empty()) {
                    allow += ", ";
                }

                allow += toString(method);
            }

            if (!allow.empty()) {
                context.response.setHeader("Allow", allow);
            }

            return;
        }

        context.request.setRouteParams(std::move(plan.params));
        context.executionPlan = std::move(plan);

        next();
    }
}

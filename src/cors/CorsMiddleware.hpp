#pragma once

#include <mach/detail/cors/CorsOptions.hpp>

namespace mach
{
    struct Context;
    class Next;
}

namespace mach::detail::cors
{
    class CorsMiddleware {

    public:
        explicit CorsMiddleware(cors::CorsOptions& options);

        void invoke(mach::Context& context, mach::Next& next);

    private:
        void addCorsHeaders(mach::Context& context, bool preflight = false) const;
        void handleOptionsRequest(mach::Context& context, const std::string& origin) const;

        void buildCachedHeaders();

        std::string joinMethods() const;
        static std::string joinStrings(const std::unordered_set<std::string>& values);

        cors::CorsOptions m_options;

        std::string m_allowedHeaders;
        std::string m_allowedMethods;
        std::string m_exposedHeaders;
        std::string m_maxAge;
    };
}

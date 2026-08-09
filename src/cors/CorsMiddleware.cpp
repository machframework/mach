#include "CorsMiddleware.hpp"

#include <string>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/middleware/Next.hpp>

#include "http/HttpUtils.hpp"
#include "utility/StringUtils.hpp"

namespace mach::detail::cors
{
    CorsMiddleware::CorsMiddleware(cors::CorsOptions& options) : m_options(options) {
        buildCachedHeaders();
    }

    void CorsMiddleware::invoke(mach::Context& context, mach::Next& next) {
        auto& request = context.request;
        
        const auto origin = request.header("origin");
        if (!origin) {
            next();
            return;
        }

        std::string originValue = std::string(*origin);

        // handle OPTIONS preflight
        if (request.method() == mach::http::Method::Options) {
            handleOptionsRequest(context, originValue);
            return;
        }

        // verify origin
        if (!m_options.allowAnyOrigin && !m_options.allowedOrigins.contains(originValue)) {
            next();
            return;
        }

        // verify method
        if (!m_options.allowAnyMethod && !m_options.allowedMethods.contains(request.method())) {
            next();
            return;
        }

        next();
        addCorsHeaders(context);
    }

    void CorsMiddleware::addCorsHeaders(mach::Context& context, bool preflight) const {
        std::string origin = std::string(context.request.header("origin").value());
        std::string vary;

        if (m_options.allowAnyOrigin && !m_options.allowCredentials) {
            origin = "*";
        } else {
            vary += "Origin";
        }

        context.response.setHeader("Access-Control-Allow-Origin", origin);
        
        if (m_options.allowCredentials) {
            context.response.setHeader("Access-Control-Allow-Credentials", "true");
        }
        if (!m_exposedHeaders.empty()) {
            context.response.setHeader("Access-Control-Expose-Headers", m_exposedHeaders);
        }

        if (preflight) {
            context.response.setHeader("Access-Control-Allow-Methods", m_allowedMethods);

            if (m_options.allowAnyHeader) {
                if (const auto requestedHeaders =
                        context.request.header("Access-Control-Request-Headers")) {
                    context.response.setHeader("Access-Control-Allow-Headers", *requestedHeaders);
                    
                    if (!vary.empty()) {
                        vary += ", ";    
                    }

                    vary += "Access-Control-Request-Headers";
                }
            } else if (!m_options.allowAnyHeader && !m_allowedHeaders.empty()) {
                context.response.setHeader("Access-Control-Allow-Headers", m_allowedHeaders);
            }

            if (m_options.maxAge) {
                context.response.setHeader("Access-Control-Max-Age", m_maxAge);
            }
        }

        if (!vary.empty()) {
            context.response.setHeader("Vary", vary);
        }
    }

    void CorsMiddleware::handleOptionsRequest(mach::Context& context, const std::string& origin)
        const {
        if (!m_options.allowAnyOrigin && !m_options.allowedOrigins.contains(origin)) {
            context.response.status(mach::http::StatusCode::Forbidden);
            return;
        }
        if (const auto methodStr = context.request.header("Access-Control-Request-Method")) {
            auto method = mach::http::toMethod(*methodStr);
            if (method == mach::http::Method::Unknown) {
                context.response.status(mach::http::StatusCode::BadRequest);
                return;
            }
            if (!m_options.allowAnyMethod && !m_options.allowedMethods.contains(method)) {
                context.response.status(mach::http::StatusCode::Forbidden);
                return;
            }
        } else {
            context.response.status(mach::http::StatusCode::BadRequest);
            return;
        }

        // validate headers
        if (!m_options.allowAnyHeader) {
            if (const auto headersStr = context.request.header("Access-Control-Request-Headers")) {
                const auto headers = split(*headersStr, ',');
                for (auto header : headers) {
                    header = trim(header);

                    auto normalizedHeader = std::string(header);
                    http::toLowercaseInPlace(normalizedHeader);

                    if (!m_options.allowedHeaders.contains(normalizedHeader)) {
                        context.response.status(mach::http::StatusCode::Forbidden);
                        return;
                    }
                }
            }
        }

        context.response.status(mach::http::StatusCode::NoContent);
        addCorsHeaders(context, true);
    }

    void CorsMiddleware::buildCachedHeaders() {
        m_allowedMethods = joinMethods();
        m_allowedHeaders = joinStrings(m_options.allowedHeaders);
        m_exposedHeaders = joinStrings(m_options.exposedHeaders);

        if (m_options.maxAge) {
            m_maxAge = std::to_string(m_options.maxAge->count());
        }
    }

    std::string CorsMiddleware::joinMethods() const {
        std::unordered_set<mach::http::Method> allowedMethods;

        if (m_options.allowAnyMethod) {
            allowedMethods.insert(mach::http::allMethods.begin(), mach::http::allMethods.end());
        } else {
            allowedMethods = m_options.allowedMethods;
        }
        
        std::string result;

        bool first = true;
        for (auto method : allowedMethods) {
            if (!first) {
                result += ", ";
            }

            result += toString(method);
            first = false;
        }

        return result;
    }

    std::string CorsMiddleware::joinStrings(const std::unordered_set<std::string>& values) {
        std::string result;

        bool first = true;
        for (const auto& value : values) {
            if (!first) {
                result += ", ";
            }

            result += value;
            first = false;
        }

        return result;
    }
}

#pragma once

#include <initializer_list>
#include <string_view>

#include <mach/http/Method.hpp>

#include <mach/detail/cors/CorsOptions.hpp>

namespace mach
{
    class AppBuilder;

    class CorsBuilder {

    public:
        CorsBuilder() = default;

        CorsBuilder(const CorsBuilder&) = delete;
        CorsBuilder& operator=(const CorsBuilder&) = delete;

        CorsBuilder(CorsBuilder&&) noexcept = default;
        CorsBuilder& operator=(CorsBuilder&&) noexcept = default;

        ~CorsBuilder() = default;

        CorsBuilder& allowOrigin(std::string_view origin);
        CorsBuilder& allowOrigins(std::initializer_list<std::string_view> origins);

        CorsBuilder& allowMethod(http::Method method);
        CorsBuilder& allowMethods(std::initializer_list<http::Method> methods);

        CorsBuilder& allowHeader(std::string_view header);
        CorsBuilder& allowHeaders(std::initializer_list<std::string_view> headers);

        CorsBuilder& exposeHeader(std::string_view header);
        CorsBuilder& exposeHeaders(std::initializer_list<std::string_view> headers);

        CorsBuilder& allowAnyOrigin();
        CorsBuilder& allowAnyHeader();
        CorsBuilder& allowAnyMethod();

        CorsBuilder& allowCredentials(bool enabled = true);
        CorsBuilder& maxAge(std::chrono::seconds maxAge);

    private:
        detail::cors::CorsOptions m_options;

        detail::cors::CorsOptions&& takeOptions() &&;

        friend class AppBuilder;
    };
}

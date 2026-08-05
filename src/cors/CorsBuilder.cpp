#include <mach/CorsBuilder.hpp>

namespace mach
{
    CorsBuilder& CorsBuilder::allowOrigin(std::string_view origin){
        m_options.allowedOrigins.emplace(origin);
        return *this;
    }

    CorsBuilder& CorsBuilder::allowOrigins(std::initializer_list<std::string_view> origins) {
        for (auto origin : origins) {
            this->allowOrigin(origin);
        }

        return *this;
    }

    CorsBuilder& CorsBuilder::allowMethod(http::Method method) {
        m_options.allowedMethods.emplace(method);
        return *this;
    }

    CorsBuilder& CorsBuilder::allowMethods(std::initializer_list<http::Method> methods) {
        for (auto method : methods) {
            this->allowMethod(method);
        }

        return *this;
    }

    CorsBuilder& CorsBuilder::allowHeader(std::string_view header){
        m_options.allowedHeaders.emplace(header);
        return *this;
    }

    CorsBuilder& CorsBuilder::allowHeaders(std::initializer_list<std::string_view> headers) {
        for (auto header : headers) {
            this->allowHeader(header);
        }

        return *this;
    }

    CorsBuilder& CorsBuilder::exposeHeader(std::string_view header) {
        m_options.exposedHeaders.emplace(header);
        return *this;
    }

    CorsBuilder& CorsBuilder::exposeHeaders(std::initializer_list<std::string_view> headers) {
        for (auto header : headers) {
            this->exposeHeader(header);
        }

        return *this;
    }

    CorsBuilder& CorsBuilder::allowAnyOrigin() {
        m_options.allowAnyOrigin = true;
        m_options.allowedOrigins.clear();
        return *this;
    }

    CorsBuilder& CorsBuilder::allowAnyHeader() {
        m_options.allowAnyHeader= true;
        m_options.allowedHeaders.clear();
        return *this;
    }

    CorsBuilder& CorsBuilder::allowAnyMethod() {
        m_options.allowAnyMethod = true;
        m_options.allowedMethods.clear();
        return *this;
    }

    CorsBuilder& CorsBuilder::allowCredentials(bool enabled) {
        m_options.allowCredentials = enabled;
        return *this;
    }

    CorsBuilder& CorsBuilder::maxAge(std::chrono::seconds maxAge) {
        m_options.maxAge = maxAge;
        return *this;
    }

    detail::cors::CorsOptions&& CorsBuilder::takeOptions() && {
        return std::move(m_options);
    }
}

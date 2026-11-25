#include <mach/Request.hpp>

#include <format>
#include <stdexcept>

#include <mach/detail/http/MediaType.hpp>

#include "AuthorizationHeader.hpp"
#include "utility/StringUtils.hpp"
#include "ContentLength.hpp"

namespace mach
{
    Request::Request(
        http::Method method,
        http::Version version,
        std::string target,
        std::string body,
        std::unordered_map<std::string, std::string> headers,
        std::unordered_map<std::string, std::string> cookies)
        : m_version(version), m_method(method), m_target(std::move(target)),
          m_body(std::move(body)), m_headers(std::move(headers)), m_cookies(std::move(cookies)) {}

    http::Version Request::version() const noexcept {
        return m_version;
    }

    http::Method Request::method() const noexcept {
        return m_method;
    }

    std::string_view Request::target() const noexcept {
        return m_target;
    }

    const std::string& Request::body() const noexcept {
        return m_body;
    }

    // Generic header access

    bool Request::containsHeader(std::string_view name) const {
        return this->header(name).has_value();
    }

    std::optional<std::string_view> Request::header(std::string_view name) const {
        auto normalizedName = std::string(name);
        detail::toLowercaseInPlace(normalizedName);

        if (const auto it = m_headers.find(normalizedName); it != m_headers.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    // Header convenience methods

    std::optional<std::string_view> Request::contentType() const {
        return this->header("Content-Type");
    }

    std::optional<std::string_view> Request::contentLength() const {
        return this->header("Content-Length");
    }

    std::optional<std::string_view> Request::contentEncoding() const {
        return this->header("Content-Encoding");
    }

    std::optional<std::string_view> Request::contentLanguage() const {
        return this->header("Content-Language");
    }

    std::optional<std::string_view> Request::accept() const {
        return this->header("Accept");
    }

    std::optional<std::string_view> Request::acceptEncoding() const {
        return this->header("Accept-Encoding");
    }

    std::optional<std::string_view> Request::acceptLanguage() const {
        return this->header("Accept-Language");
    }

    std::optional<std::string_view> Request::authorization() const {
        return this->header("Authorization");
    }

    std::optional<std::string_view> Request::host() const {
        return this->header("Host");
    }

    std::optional<std::string_view> Request::userAgent() const {
        return this->header("User-Agent");
    }

    std::optional<std::string_view> Request::origin() const {
        return this->header("Origin");
    }

    std::optional<std::string_view> Request::referer() const {
        return this->header("Referer");
    }

    std::optional<std::string_view> Request::cacheControl() const {
        return this->header("Cache-Control");
    }

    std::optional<std::string_view> Request::ifMatch() const {
        return this->header("If-Match");
    }

    std::optional<std::string_view> Request::ifNoneMatch() const {
        return this->header("If-None-Match");
    }

    std::optional<std::string_view> Request::ifModifiedSince() const {
        return this->header("If-Modified-Since");
    }

    std::optional<std::string_view> Request::ifUnmodifiedSince() const {
        return this->header("If-Unmodified-Since");
    }

    std::optional<std::string_view> Request::range() const {
        return this->header("Range");
    }

    std::optional<std::string_view> Request::ifRange() const {
        return this->header("If-Range");
    }

    std::optional<std::string_view> Request::forwarded() const {
        return this->header("Forwarded");
    }

    std::optional<std::string_view> Request::xForwardedFor() const {
        return this->header("X-Forwarded-For");
    }

    std::optional<std::string_view> Request::xForwardedHost() const {
        return this->header("X-Forwarded-Host");
    }

    std::optional<std::string_view> Request::xForwardedProto() const {
        return this->header("X-Forwarded-Proto");
    }

    std::optional<std::string_view> Request::mediaType() const {
        const auto value = this->contentType();
        return value ? detail::http::extractMediaType(*value) : std::nullopt;
    }

    std::optional<std::string_view> Request::charset() const {
        const auto value = this->contentType();
        return value ? detail::http::extractCharset(*value) : std::nullopt;
    }

    bool Request::isContentType(std::string_view type) const {
        const auto value = this->contentType();
        return value && detail::http::matchesMediaType(*value, type);
    }

    bool Request::isJson() const {
        const auto value = this->contentType();
        return value && detail::http::isJsonMediaType(*value);
    }

    bool Request::isMultipartFormData() const {
        return this->isContentType("multipart/form-data");
    }

    bool Request::isFormUrlEncoded() const {
        return this->isContentType("application/x-www-form-urlencoded");
    }

    std::optional<std::uint64_t> Request::contentLengthBytes() const {
        const auto value = this->contentLength();
        return value ? detail::http::parseContentLength(*value) : std::nullopt;
    }

    std::optional<std::string_view> Request::authorizationScheme() const {
        const auto value = this->authorization();

        return value ? detail::http::extractAuthorizationScheme(*value) : std::nullopt;
    }

    std::optional<std::string_view> Request::bearerToken() const {
        const auto value = this->authorization();

        return value ? detail::http::extractBearerToken(*value) : std::nullopt;
    }

    // Route parameters

    std::string_view Request::routeParam(std::string_view name) const {
        if (const auto it = m_routeParams.find(std::string(name)); it != m_routeParams.end()) {
            return it->second;
        }

        throw std::out_of_range(std::format("Route parameter '{}' does not exist", name));
    }

    // Cookies

    bool Request::containsCookie(std::string_view name) const {
        return this->cookie(name).has_value();
    }

    std::optional<std::string_view> Request::cookie(std::string_view name) const {
        if (const auto it = m_cookies.find(std::string(name)); it != m_cookies.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    // Query parameters

    bool Request::containsQuery(std::string_view name) const {
        return this->query(name).has_value();
    }

    std::optional<std::string_view> Request::query(std::string_view name) const {
        if (const auto it = m_query.find(std::string(name)); it != m_query.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    // Internal setters

    void Request::setRouteParams(std::unordered_map<std::string, std::string>&& params) {
        m_routeParams = std::move(params);
    }

    void Request::setRouteQuery(std::unordered_map<std::string, std::string>&& query) {
        m_query = std::move(query);
    }
}

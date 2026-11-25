#include <mach/Response.hpp>

#include <algorithm>
#include <format>
#include <stdexcept>
#include <unordered_set>

#include <mach/detail/http/MediaType.hpp>

#include "ContentDisposition.hpp"
#include "ContentLength.hpp"
#include "utility/StringUtils.hpp"
#include "utility/HttpDate.hpp"

namespace
{
    inline const std::unordered_set<std::string_view> reservedResponseHeaders = {
        "connection",
        "keep-alive",
        "transfer-encoding",
        "content-length",
        "set-cookie",
        "trailer",
        "upgrade",
        "proxy-connection"};

    bool isValidHeaderName(std::string_view name) noexcept {
        return !name.empty() && std::ranges::all_of(name, [](unsigned char c) {
            return std::isalnum(c) || c == '!' || c == '#' || c == '$' || c == '%' || c == '&' ||
                   c == '\'' || c == '*' || c == '+' || c == '-' || c == '.' || c == '^' ||
                   c == '_' || c == '`' || c == '|' || c == '~';
        });
    }

    bool containsCrOrLf(std::string_view value) noexcept {
        return value.find('\r') != std::string_view::npos ||
               value.find('\n') != std::string_view::npos;
    }
}

namespace mach
{
    Response::Response(http::Version version, http::StatusCode status)
        : m_version(version), m_status(status) {}

    // Response properties

    http::Version Response::version() const noexcept {
        return m_version;
    }

    http::StatusCode Response::status() const noexcept {
        return m_status;
    }

    const std::string& Response::body() const noexcept {
        return m_body;
    }

    void Response::status(http::StatusCode status) {
        if (!http::isValidStatusCode(status)) {
            throw std::invalid_argument("Invalid HTTP status code");
        }

        m_status = status;
    }

    void Response::body(std::string body) noexcept {
        m_body = std::move(body);
    }

    // Generic header access

    bool Response::containsHeader(std::string_view name) const {
        auto normalizedName = std::string(name);
        detail::toLowercaseInPlace(normalizedName);

        return m_headers.contains(normalizedName);
    }

    std::optional<std::string_view> Response::header(std::string_view name) const {
        auto normalizedName = std::string(name);
        detail::toLowercaseInPlace(normalizedName);

        if (const auto it = m_headers.find(normalizedName); it != m_headers.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    void Response::setHeader(std::string_view name, std::string_view value) {
        auto normalizedName = std::string(name);
        detail::toLowercaseInPlace(normalizedName);

        if (normalizedName.empty()) {
            throw std::invalid_argument("Header name cannot be empty");
        }
        if (!isValidHeaderName(normalizedName)) {
            throw std::invalid_argument(std::format("Invalid header name '{}'.", normalizedName));
        }
        if (containsCrOrLf(value)) {
            throw std::invalid_argument(
                std::format(
                    "Header '{}' value cannot contain CR or LF characters.",
                    normalizedName));
        }
        if (reservedResponseHeaders.contains(normalizedName)) {
            throw std::invalid_argument(
                std::format(
                    "The '{}' header is managed by Mach and cannot be set manually.",
                    normalizedName));
        }

        m_headers.insert_or_assign(std::move(normalizedName), std::string(value));
    }

    // Header convenience methods

    std::optional<std::string_view> Response::contentType() const {
        return this->header("Content-Type");
    }

    std::optional<std::string_view> Response::contentLength() const {
        return this->header("Content-Length");
    }

    std::optional<std::string_view> Response::contentEncoding() const {
        return this->header("Content-Encoding");
    }

    std::optional<std::string_view> Response::contentLanguage() const {
        return this->header("Content-Language");
    }

    std::optional<std::string_view> Response::contentDisposition() const {
        return this->header("Content-Disposition");
    }

    std::optional<std::string_view> Response::contentRange() const {
        return this->header("Content-Range");
    }

    std::optional<std::string_view> Response::acceptRanges() const {
        return this->header("Accept-Ranges");
    }

    std::optional<std::string_view> Response::cacheControl() const {
        return this->header("Cache-Control");
    }

    std::optional<std::string_view> Response::etag() const {
        return this->header("ETag");
    }

    std::optional<std::string_view> Response::lastModified() const {
        return this->header("Last-Modified");
    }

    std::optional<std::string_view> Response::expires() const {
        return this->header("Expires");
    }

    std::optional<std::string_view> Response::vary() const {
        return this->header("Vary");
    }

    std::optional<std::string_view> Response::age() const {
        return this->header("Age");
    }

    std::optional<std::string_view> Response::wwwAuthenticate() const {
        return this->header("WWW-Authenticate");
    }

    std::optional<std::string_view> Response::location() const {
        return this->header("Location");
    }

    std::optional<std::string_view> Response::retryAfter() const {
        return this->header("Retry-After");
    }

    std::optional<std::string_view> Response::allow() const {
        return this->header("Allow");
    }

    std::optional<std::string_view> Response::link() const {
        return this->header("Link");
    }

    std::optional<std::string_view> Response::server() const {
        return this->header("Server");
    }

    std::optional<std::string_view> Response::date() const {
        return this->header("Date");
    }

    std::optional<std::string_view> Response::contentSecurityPolicy() const {
        return this->header("Content-Security-Policy");
    }

    std::optional<std::string_view> Response::contentSecurityPolicyReportOnly() const {
        return this->header("Content-Security-Policy-Report-Only");
    }

    std::optional<std::string_view> Response::strictTransportSecurity() const {
        return this->header("Strict-Transport-Security");
    }

    std::optional<std::string_view> Response::xContentTypeOptions() const {
        return this->header("X-Content-Type-Options");
    }

    std::optional<std::string_view> Response::xFrameOptions() const {
        return this->header("X-Frame-Options");
    }

    std::optional<std::string_view> Response::referrerPolicy() const {
        return this->header("Referrer-Policy");
    }

    std::optional<std::string_view> Response::permissionsPolicy() const {
        return this->header("Permissions-Policy");
    }

    void Response::setContentType(std::string_view value) {
        this->setHeader("Content-Type", value);
    }

    void Response::setContentEncoding(std::string_view value) {
        this->setHeader("Content-Encoding", value);
    }

    void Response::setContentLanguage(std::string_view value) {
        this->setHeader("Content-Language", value);
    }

    void Response::setContentDisposition(std::string_view value) {
        this->setHeader("Content-Disposition", value);
    }

    void Response::setContentRange(std::string_view value) {
        this->setHeader("Content-Range", value);
    }

    void Response::setAcceptRanges(std::string_view value) {
        this->setHeader("Accept-Ranges", value);
    }

    void Response::setCacheControl(std::string_view value) {
        this->setHeader("Cache-Control", value);
    }

    void Response::setEtag(std::string_view value) {
        this->setHeader("ETag", value);
    }

    void Response::setLastModified(std::string_view value) {
        this->setHeader("Last-Modified", value);
    }

    void Response::setLastModified(std::chrono::system_clock::time_point value) {
        this->setLastModified(detail::formatHttpDate(value));
    }

    void Response::setExpires(std::string_view value) {
        this->setHeader("Expires", value);
    }

    void Response::setExpires(std::chrono::system_clock::time_point value) {
        this->setExpires(detail::formatHttpDate(value));
    }

    void Response::setVary(std::string_view value) {
        this->setHeader("Vary", value);
    }

    void Response::setAge(std::string_view value) {
        this->setHeader("Age", value);
    }

    void Response::setAge(std::chrono::seconds value) {
        if (value.count() < 0) {
            throw std::invalid_argument("Age cannot be negative");
        }

        this->setAge(std::to_string(value.count()));
    }

    void Response::setWwwAuthenticate(std::string_view value) {
        this->setHeader("WWW-Authenticate", value);
    }

    void Response::setLocation(std::string_view value) {
        this->setHeader("Location", value);
    }

    void Response::setRetryAfter(std::string_view value) {
        this->setHeader("Retry-After", value);
    }

    void Response::setRetryAfter(std::chrono::seconds value) {
        if (value.count() < 0) {
            throw std::invalid_argument("Retry-After cannot be negative");
        }

        this->setRetryAfter(std::to_string(value.count()));
    }

    void Response::setRetryAfter(std::chrono::system_clock::time_point value) {
        this->setRetryAfter(detail::formatHttpDate(value));
    }

    void Response::setAllow(std::string_view value) {
        this->setHeader("Allow", value);
    }

    void Response::setAllow(std::initializer_list<http::Method> methods) {
        std::string value;

        for (const auto method : methods) {
            const auto name = http::toString(method);

            if (name == "UNKNOWN") {
                throw std::invalid_argument("Invalid HTTP method");
            }

            if (!value.empty()) {
                value += ", ";
            }

            value += name;
        }

        this->setAllow(value);
    }

    void Response::setLink(std::string_view value) {
        this->setHeader("Link", value);
    }

    void Response::setServer(std::string_view value) {
        this->setHeader("Server", value);
    }

    void Response::setDate(std::string_view value) {
        this->setHeader("Date", value);
    }

    void Response::setDate(std::chrono::system_clock::time_point value) {
        this->setDate(detail::formatHttpDate(value));
    }

    void Response::setContentSecurityPolicy(std::string_view value) {
        this->setHeader("Content-Security-Policy", value);
    }

    void Response::setContentSecurityPolicyReportOnly(std::string_view value) {
        this->setHeader("Content-Security-Policy-Report-Only", value);
    }

    void Response::setStrictTransportSecurity(std::string_view value) {
        this->setHeader("Strict-Transport-Security", value);
    }

    void Response::setXContentTypeOptions(std::string_view value) {
        this->setHeader("X-Content-Type-Options", value);
    }

    void Response::setXFrameOptions(std::string_view value) {
        this->setHeader("X-Frame-Options", value);
    }

    void Response::setReferrerPolicy(std::string_view value) {
        this->setHeader("Referrer-Policy", value);
    }

    void Response::setPermissionsPolicy(std::string_view value) {
        this->setHeader("Permissions-Policy", value);
    }

    std::optional<std::string_view> Response::mediaType() const {
        const auto value = this->contentType();

        return value ? detail::http::extractMediaType(*value) : std::nullopt;
    }

    std::optional<std::string_view> Response::charset() const {
        const auto value = this->contentType();

        return value ? detail::http::extractCharset(*value) : std::nullopt;
    }

    bool Response::isContentType(std::string_view type) const {
        const auto value = this->contentType();

        return value && detail::http::matchesMediaType(*value, type);
    }

    bool Response::isJson() const {
        const auto value = this->contentType();

        return value && detail::http::isJsonMediaType(*value);
    }

    bool Response::isMultipartFormData() const {
        return this->isContentType("multipart/form-data");
    }

    bool Response::isFormUrlEncoded() const {
        return this->isContentType("application/x-www-form-urlencoded");
    }

    std::optional<std::uint64_t> Response::contentLengthBytes() const {
        const auto value = this->contentLength();
        return value ? detail::http::parseContentLength(*value) : std::nullopt;
    }

    void Response::setAttachment(std::string_view filename) {
        this->setContentDisposition(detail::http::formatContentDisposition("attachment", filename));
    }

    void Response::setInline(std::string_view filename) {
        this->setContentDisposition(detail::http::formatContentDisposition("inline", filename));
    }

    // Cookies

    bool Response::containsCookie(std::string_view name) const noexcept {
        return this->cookie(name).has_value();
    }

    std::optional<mach::http::Cookie> Response::cookie(std::string_view name) const {
        for (const auto& cookie : m_cookies) {
            if (cookie.name == name) {
                return cookie;
            }
        }

        return std::nullopt;
    }

    void Response::addCookie(const mach::http::Cookie& cookie) {
        m_cookies.push_back(cookie);
    }

    void Response::addCookie(mach::http::Cookie&& cookie) {
        m_cookies.push_back(std::move(cookie));
    }

    void Response::addCookie(std::string name, std::string value) {
        mach::http::Cookie cookie{
            .name = std::move(name),
            .value = std::move(value),
        };

        this->addCookie(std::move(cookie));
    }
}

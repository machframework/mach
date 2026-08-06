#include "mach/Response.hpp"

#include <format>
#include <stdexcept>
#include <unordered_set>

#include "HttpUtils.hpp"

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

    inline bool isValidHeaderName(std::string_view name) noexcept {
        for (unsigned char c : name) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '!' && c != '#' && c != '$' &&
                c != '%' && c != '&' && c != '\'' && c != '*' && c != '+' && c != '-' && c != '.' &&
                c != '^' && c != '_' && c != '`' && c != '|' && c != '~') {
                return false;
            }
        }

        return true;
    }

    inline bool containsCrOrLf(std::string_view value) noexcept {
        return value.find('\r') != std::string_view::npos ||
               value.find('\n') != std::string_view::npos;
    }
}

namespace mach
{
    Response::Response(http::Version version, http::StatusCode status)
        : m_version(version), m_status(status) {}

    http::Version Response::version() const noexcept {
        return m_version;
    }

    http::StatusCode Response::status() const noexcept {
        return m_status;
    }

    const std::string& Response::body() const noexcept {
        return m_body;
    }

    std::optional<std::string_view> Response::header(std::string_view name) const {
        std::string normalizedName = std::string(name);
        detail::http::toLowercaseInPlace(normalizedName);

        auto it = m_headers.find(normalizedName);
        if (it != m_headers.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    bool Response::containsHeader(std::string_view name) const noexcept {
        std::string normalizedName = std::string(name);
        detail::http::toLowercaseInPlace(normalizedName);

        return m_headers.find(normalizedName) != m_headers.end();
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

    void Response::setHeader(std::string_view name, std::string_view value) {
        std::string normalizedName = std::string(name);
        detail::http::toLowercaseInPlace(normalizedName);

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

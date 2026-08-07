#include "mach/Request.hpp"

#include "HttpUtils.hpp"

#include <format>
#include <optional>
#include <stdexcept>

namespace mach
{
    Request::Request(
        http::Method method,
        http::Version version,
        std::string target,
        std::string body,
        std::unordered_map<std::string, std::string> headers,
        std::unordered_map<std::string, std::string> cookies)
        : m_method(method), m_version(version), m_target(std::move(target)),
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

    std::optional<std::string_view> Request::header(std::string_view name) const {
        std::string normalizedName = std::string(name);
        detail::http::toLowercaseInPlace(normalizedName);

        auto it = m_headers.find(normalizedName);
        if (it != m_headers.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    bool Request::containsHeader(std::string_view name) const noexcept {
        return this->header(name).has_value();
    }

    std::string_view Request::routeParam(std::string_view name) const {
        auto it = m_routeParams.find(std::string(name));
        if (it != m_routeParams.end()) {
            return it->second;
        }

        throw std::out_of_range(std::format("Route parameter '{}' does not exist", name));
    }

    bool Request::containsCookie(std::string_view name) const noexcept {
        return this->cookie(name).has_value();
    }

    std::optional<std::string_view> Request::cookie(std::string_view name) const {
        auto it = m_cookies.find(std::string(name));
        if (it != m_cookies.end()) {
            return it->second;
        }

        return std::nullopt;
    }

    void Request::setRouteParams(std::unordered_map<std::string, std::string>&& params) {
        m_routeParams = std::move(params);
    }

    void Request::setRouteQuery(std::unordered_map<std::string, std::string>&& query) {
        m_query = std::move(query);
    }
}

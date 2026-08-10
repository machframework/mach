#include "BeastResponseAdapter.hpp"

#include <format>

#include <mach/http/Cookie.hpp>

#include "utility/HttpDate.hpp"

namespace mach::detail::http::adapter
{
    beast::http::response<beast::http::string_body> BeastResponseAdapter::adapt(
        mach::Context&& context) const {
        beast::http::response<beast::http::string_body> res;

        const auto version = context.response.version();
        const auto beastVersion = fromMachVersion(version);
        res.version(beastVersion);
        res.result(static_cast<unsigned int>(context.response.status()));

        res.body() = context.response.body();

        for (const auto& [name, value] : context.response.m_headers) {
            res.set(name, value);
        }

        writeCookies(res, context.response.m_cookies);
        return res;
    }

    void BeastResponseAdapter::writeCookies(
        beast::http::response<beast::http::string_body>& response,
        const std::vector<mach::http::Cookie>& cookies) {
        for (const auto& cookie : cookies) {
            response.set(beast::http::field::set_cookie, cookieToString(cookie));
        }
    }

    std::string BeastResponseAdapter::cookieToString(const mach::http::Cookie& cookie) {
        auto cookieStr = std::format("{}={};", cookie.name, cookie.value);

        if (const auto& expires = cookie.expires) {
            cookieStr += std::format(" Expires={};", formatHttpDate(*expires));
        }
        if (const auto& maxAge = cookie.maxAge) {
            cookieStr += std::format(" Max-Age={};", maxAge->count());
        }
        if (const auto& domain = cookie.domain) {
            cookieStr += std::format(" Domain={};", *domain);
        }

        cookieStr += std::format(" Path={};", cookie.path);

        if (cookie.secure) {
            cookieStr += " Secure;";
        }
        if (cookie.httpOnly) {
            cookieStr += " HttpOnly;";
        }

        switch (cookie.sameSite) {
        case mach::http::SameSite::Strict:
            cookieStr += " SameSite=Strict";
            break;

        case mach::http::SameSite::Lax:
            cookieStr += " SameSite=Lax";
            break;

        case mach::http::SameSite::None:
            cookieStr += " SameSite=None";
            break;
        }

        return cookieStr;
    }

    unsigned int BeastResponseAdapter::fromMachVersion(mach::http::Version version) {
        switch (version) {
        case mach::http::Version::Http10:
            return 10;
        case mach::http::Version::Http11:
            return 11;
        case mach::http::Version::Http2:
            return 20;
        case mach::http::Version::Http3:
            return 30;
        default:
            return 0; // unknown
        }
    }
}

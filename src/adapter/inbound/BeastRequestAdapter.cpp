#include "BeastRequestAdapter.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/Method.hpp>
#include <mach/http/StatusCode.hpp>

#include "http/HttpUtils.hpp"
#include "utility/StringUtils.hpp"

namespace
{
    bool containsControlCharacters(std::string_view target) {
        for (unsigned char c : target) {
            if (std::iscntrl(c)) {
                return true;
            }
        }

        return false;
    }
}
namespace mach::detail::http::adapter
{
    mach::Context BeastRequestAdapter::adapt(
        beast::http::request<beast::http::string_body>&& rawRequest,
        bool& adapterRejectedRequest) {
        auto version = fromBeastVersion(rawRequest.version());
        auto method = fromBeastVerb(rawRequest.method());
        std::string target(rawRequest.target());

        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<std::string, std::string> cookies;

        for (auto const& field : rawRequest.base()) {
            auto key = std::string(field.name_string());
            auto value = field.value();

            toLowercaseInPlace(key);

            if (field.name() == beast::http::field::cookie) {
                extractCookiesFromHeader(value, cookies);
            } else {
                headers.insert_or_assign(std::move(key), std::string(value));
            }
        }

        mach::Request req(
            method,
            version,
            std::move(target),
            std::move(rawRequest.body()),
            std::move(headers),
            std::move(cookies));

        // create an empty response
        mach::Response res(version);

        if (version == mach::http::Version::Unknown) {
            adapterRejectedRequest = true;
            res.status(mach::http::StatusCode::HttpVersionNotSupported);
            res.body("Unsupported HTTP version.");
        } else if (method == mach::http::Method::Unknown) {
            adapterRejectedRequest = true;
            res.status(mach::http::StatusCode::NotImplemented);
            res.body("Unsupported HTTP method.");
        } else if (req.target().empty()) {
            adapterRejectedRequest = true;
            res.status(mach::http::StatusCode::BadRequest);
            res.body("Request target must not be empty.");
        } else if (req.target().front() != '/') {
            adapterRejectedRequest = true;
            res.status(mach::http::StatusCode::BadRequest);
            res.body("Request target must start with '/'.");
        } else if (containsControlCharacters(req.target())) {
            adapterRejectedRequest = true;
            res.status(mach::http::StatusCode::BadRequest);
            res.body("Request target contains control characters.");
        }

        return mach::Context(std::move(req), std::move(res));
    }

    void BeastRequestAdapter::extractCookiesFromHeader(
        std::string_view value,
        std::unordered_map<std::string, std::string>& cookies) const {
        auto newCookies = detail::split(value, ';');

        for (std::string_view cookie : newCookies) {
            std::size_t equalPos = cookie.find('=');
            if (equalPos != std::string_view::npos) {
                std::string_view name = detail::trim(cookie.substr(0, equalPos));
                std::string_view val = detail::trim(cookie.substr(equalPos + 1));
                if (!name.empty()) {
                    cookies.emplace(std::string(name), std::string(val));
                }
            }
        }
    }

    mach::http::Method BeastRequestAdapter::fromBeastVerb(beast::http::verb verb) {
        switch (verb) {
        case beast::http::verb::get:
            return mach::http::Method::Get;
        case beast::http::verb::post:
            return mach::http::Method::Post;
        case beast::http::verb::put:
            return mach::http::Method::Put;
        case beast::http::verb::patch:
            return mach::http::Method::Patch;
        case beast::http::verb::delete_:
            return mach::http::Method::Delete;
        case beast::http::verb::head:
            return mach::http::Method::Head;
        case beast::http::verb::options:
            return mach::http::Method::Options;
        default:
            return mach::http::Method::Unknown;
        }
    }

    mach::http::Version BeastRequestAdapter::fromBeastVersion(unsigned int version) {
        // Mach currently supports HTTP/1.0 and HTTP/1.1 only.
        switch (version) {
        case 10:
            return mach::http::Version::Http10;
        case 11:
            return mach::http::Version::Http11;
        default:
            return mach::http::Version::Unknown;
        }
    }
}

#include "BeastResponseAdapter.hpp"

#include <mach/http/StatusCode.hpp>

namespace mach::detail::http::adapter
{
    beast::http::response<beast::http::string_body> BeastResponseAdapter::adapt(
        mach::Context&& context) {
        beast::http::response<beast::http::string_body> res;

        auto version = context.response.version();
        auto beastVersion = fromMachVersion(version);
        res.version(beastVersion);
        res.result(static_cast<unsigned int>(context.response.status()));

        res.body() = context.response.body();

        for (const auto& [name, value] : context.response.m_headers) {
            res.set(name, value);
        }

        return res;
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

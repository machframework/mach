#pragma once

#include <vector>

#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body.hpp>

#include <mach/Context.hpp>
#include <mach/http/Version.hpp>

namespace mach::http
{
    struct Cookie;
}

namespace mach::detail::http::adapter
{
    namespace beast = boost::beast;

    class BeastResponseAdapter {

    public:
        [[nodiscard]] beast::http::response<beast::http::string_body> adapt(mach::Context&& context) const;

    private:
        void writeCookies(
            beast::http::response<beast::http::string_body>& response,
            const std::vector<mach::http::Cookie>& cookies) const;

        std::string cookieToString(const mach::http::Cookie& cookie) const;

        static unsigned int fromMachVersion(mach::http::Version version);
    };
}

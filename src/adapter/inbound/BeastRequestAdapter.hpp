#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body.hpp>

#include <mach/Context.hpp>
#include <mach/http/Cookie.hpp>
#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

namespace mach::detail::http::adapter
{
    namespace beast = boost::beast;

    class BeastRequestAdapter {

    public:
        mach::Context adapt(
            beast::http::request<beast::http::string_body>&& rawRequest,
            bool& adapterRejectedRequest);

    private:
        void extractCookiesFromHeader(
            std::string_view value,
            std::unordered_map<std::string, std::string>& cookies) const;

        mach::http::Method fromBeastVerb(beast::http::verb verb);
        mach::http::Version fromBeastVersion(unsigned int version);
    };
}

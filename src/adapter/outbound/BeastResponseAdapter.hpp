#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast/http/message_generator.hpp>
#include <boost/beast/http/string_body.hpp>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

namespace mach::detail::http::adapter
{
    namespace beast = boost::beast;

    class BeastResponseAdapter {

    public:
        beast::http::response<beast::http::string_body> adapt(mach::Context&& context);

    private:
        static unsigned int fromMachVersion(mach::http::Version version);
    };
}

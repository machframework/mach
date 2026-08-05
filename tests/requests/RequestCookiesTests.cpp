#include "Testing.hpp"

#include "adapter/inbound/BeastRequestAdapter.hpp"
#include <mach/Request.hpp>


int main() {
    mach::detail::http::adapter::BeastRequestAdapter adapter;

    auto rawRequest = boost::beast::http::request<boost::beast::http::string_body>(
        boost::beast::http::verb::get,
        "/test",
        11);

    rawRequest.insert(boost::beast::http::field::cookie, "sessionId=def456");

    rawRequest.insert(
        boost::beast::http::field::cookie,
        "sessionId=abc123; userId=42; theme=dark; empty=; "
        "spaced = value ; =ignored; noEquals; token=hello=world; duplicate=first; "
        "duplicate=second");

    bool rejected = false;
    auto context = adapter.adapt(std::move(rawRequest), rejected);

    const auto& request = context.request;

    if (rejected) {
        return 1; // Adapter rejected the request
    }

    return 0;
}
#include "Testing.hpp"

#include <boost/beast/http.hpp>

#include "adapter/inbound/BeastRequestAdapter.hpp"

namespace http = boost::beast::http;

static http::request<http::string_body> makeRequest(
	http::verb method = http::verb::get,
	std::string target = "/",
	std::string body = ""
);

int main() {
	return 0;
}

http::request<http::string_body> makeRequest(
	http::verb method,
	std::string target,
	std::string body
)
{
	http::request<http::string_body> req;

	req.version(11);
	req.method(method);
	req.target(std::move(target));
	req.body() = std::move(body);

	req.set(http::field::host, "localhost");
	req.set(http::field::content_type, "application/json");

	req.prepare_payload();
	return req;
}

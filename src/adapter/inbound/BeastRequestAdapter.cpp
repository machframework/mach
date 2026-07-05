#include "BeastRequestAdapter.hpp"

#include <mach/http/Method.hpp>
#include <mach/http/StatusCode.hpp>

#include <unordered_map>

namespace mach::detail::http::adapter
{
	mach::Context BeastRequestAdapter::adapt(
		beast::http::request<beast::http::string_body>&& rawRequest,
		bool& adapterRejectedRequest) 
	{
		// adapt request
		auto version = fromBeastVersion(rawRequest.version());
		
		std::unordered_map<std::string, std::string> headers;
		for (auto const& field : rawRequest.base()) {
			headers.insert_or_assign(std::string(field.name_string()), std::string(field.value()));
		};

		mach::Request req(
			fromBeastVerb(rawRequest.method()),
			version,
			std::move(std::string(rawRequest.target())),
			std::move(rawRequest.body()),
			std::move(headers)
		);

		// create an empty response
		mach::Response res(version);

		if (req.method() == mach::http::Method::Unknown) {
			res.status(mach::http::StatusCode::NotImplemented);
			adapterRejectedRequest = true;
		}

		return mach::Context(std::move(req), std::move(res));
	}

	mach::http::Method BeastRequestAdapter::fromBeastVerb(beast::http::verb verb) {
		switch (verb) {
		case beast::http::verb::get: return mach::http::Method::Get;
		case beast::http::verb::post: return mach::http::Method::Post;
		case beast::http::verb::put: return mach::http::Method::Put;
		case beast::http::verb::patch: return mach::http::Method::Patch;
		case beast::http::verb::delete_: return mach::http::Method::Delete;
		case beast::http::verb::head: return mach::http::Method::Head;
		case beast::http::verb::options: return mach::http::Method::Options;
		default: return mach::http::Method::Unknown;
		}
	}

	mach::http::Version BeastRequestAdapter::fromBeastVersion(unsigned int version) {
		switch (version) {
		case 10: return mach::http::Version::Http10;
		case 11: return mach::http::Version::Http11;
		case 20: return mach::http::Version::Http2;
		case 30: return mach::http::Version::Http3;
		default: return mach::http::Version::Unknown;
		}
	}
}

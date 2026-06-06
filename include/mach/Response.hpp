#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/StatusCode.hpp>
#include <mach/http/Version.hpp>

namespace mach::detail::http::adapter {
	class BeastRequestAdapter;
	class BeastResponseAdapter;
}

namespace mach
{
	class Response {

	public:
		http::Version version() const noexcept;
		http::StatusCode status() const noexcept;
		const std::string& body() const noexcept;
		std::optional<std::string_view> header(std::string_view name) const;
		bool containsHeader(std::string_view name) const noexcept;

		void status(http::StatusCode status) noexcept;
		void body(std::string body) noexcept;
		void setHeader(std::string name, std::string value) noexcept;

	private:
		Response(
			http::Version version,
			http::StatusCode status = http::StatusCode::OK
		);

		http::Version m_version;
		http::StatusCode m_status;
		std::string m_body;
		std::unordered_map<std::string, std::string> m_headers;

		friend class mach::detail::http::adapter::BeastRequestAdapter;
		friend class mach::detail::http::adapter::BeastResponseAdapter;
	};
}

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

namespace mach::detail::http::adapter {
	class BeastRequestAdapter;
}

namespace mach
{
	class Request {

	public:
		http::Method method() const noexcept;
		http::Version version() const noexcept;
		std::string target() const;
		const std::string& body() const noexcept;
		std::optional<std::string_view> header(std::string_view name) const;
		bool containsHeader(std::string_view name) const noexcept;

	private:
		Request(
			http::Method method,
			http::Version version,
			std::string target,
			std::string body,
			std::unordered_map<std::string, std::string> headers
		);

		http::Version m_version;
		http::Method m_method;
		std::string m_target;
		std::string m_body;
		std::unordered_map<std::string, std::string> m_headers;

		friend class mach::detail::http::adapter::BeastRequestAdapter;
	};
}

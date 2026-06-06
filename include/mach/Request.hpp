#include <optional>
#include <string>
#include <unordered_map>

#include <mach/http/Method.hpp>
#include <mach/http/Version.hpp>

namespace mach
{
	using http::Method;

	class Request {

	public:
		http::Method method() const noexcept;
		http::Version version() const noexcept;
		std::string target() const;
		std::string body() const;
		std::optional<std::string> header(const std::string& name) const;

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
	};
}

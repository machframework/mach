#include "mach/Request.hpp"

#include <optional>

namespace mach
{
	Request::Request(
		http::Method method,
		http::Version version,
		std::string target,
		std::string body,
		std::unordered_map<std::string, std::string> headers
	) :
		m_method(method),
		m_version(version),
		m_target(std::move(target)),
		m_body(std::move(body)),
		m_headers(std::move(headers))
	{}

	mach::http::Version Request::version() const noexcept
	{
		return m_version;
	}

	mach::http::Method Request::method() const noexcept
	{
		return m_method;
	}

	std::string Request::target() const
	{
		return m_target;
	}

	std::string Request::body() const
	{
		return m_body;
	}

	std::optional<std::string> Request::header(const std::string& name) const
	{
		auto it = m_headers.find(name);
		if (it != m_headers.end()) {
			return it->second;
		}

		return std::nullopt;
	}
}
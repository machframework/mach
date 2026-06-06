#include "mach/Response.hpp"

#include "HttpUtils.hpp"

namespace mach
{
	mach::http::Version Response::version() const noexcept
	{
		return m_version;
	}

	mach::http::StatusCode Response::status() const noexcept
	{
		return m_status;
	}

	const std::string& Response::body() const noexcept
	{
		return m_body;
	}

	std::optional<std::string_view> Response::header(std::string_view name) const
	{
		std::string normalizedName = std::string(name);
		mach::detail::http::toLowercaseInPlace(normalizedName);

		auto it = m_headers.find(normalizedName);
		if (it != m_headers.end()) {
			return it->second;
		}

		return std::nullopt;
	}

	bool Response::containsHeader(std::string_view name) const noexcept
	{
		std::string normalizedName = std::string(name);
		mach::detail::http::toLowercaseInPlace(normalizedName);
		return m_headers.find(normalizedName) != m_headers.end();
	}

	void Response::status(http::StatusCode status) noexcept
	{
		m_status = status;
	}

	void Response::body(std::string body) noexcept
	{
		m_body = std::move(body);
	}

	void Response::setHeader(std::string name, std::string value) noexcept
	{
		m_headers[std::move(name)] = std::move(value);
	}
}

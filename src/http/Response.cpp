#include "mach/Response.hpp"

#include <format>
#include <stdexcept>
#include <unordered_set>

#include "HttpUtils.hpp"

namespace
{
	static const std::unordered_set<std::string_view> reservedResponseHeaders = {
	"connection",
	"keep-alive",
	"transfer-encoding",
	"content-length",
	"set-cookie",
	"trailer",
	"upgrade",
	"proxy-connection"
	};
}

namespace mach
{
	Response::Response(
		http::Version version,
		http::StatusCode status
	):
		m_version(version),
		m_status(status)
	{ }

	http::Version Response::version() const noexcept
	{
		return m_version;
	}

	http::StatusCode Response::status() const noexcept
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
		detail::http::toLowercaseInPlace(normalizedName);

		auto it = m_headers.find(normalizedName);
		if (it != m_headers.end()) {
			return it->second;
		}

		return std::nullopt;
	}

	bool Response::containsHeader(std::string_view name) const noexcept
	{
		std::string normalizedName = std::string(name);
		detail::http::toLowercaseInPlace(normalizedName);

		return m_headers.find(normalizedName) != m_headers.end();
	}

	void Response::status(http::StatusCode status)
	{
		if (!http::isValidStatusCode(status)) {
			throw std::invalid_argument("Invalid HTTP status code");
		}

		m_status = status;
	}

	void Response::body(std::string body) noexcept
	{
		m_body = std::move(body);
	}

	void Response::setHeader(std::string_view name, std::string_view value)
	{
		std::string normalizedName = std::string(name);
		detail::http::toLowercaseInPlace(normalizedName);

		if (reservedResponseHeaders.contains(normalizedName)) {
			throw std::invalid_argument(
				std::format(
					"The '{}' header is managed by Mach and cannot be set manually.", normalizedName
				)
			);
		}

		m_headers.insert_or_assign(
			std::move(normalizedName),
			std::string(value)
		);
	}
}

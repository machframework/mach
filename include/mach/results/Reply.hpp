#pragma once

#include <mach/http/StatusCode.hpp>

namespace mach
{
	template <typename T>
	class Reply {
	
	public:
		http::StatusCode statusCode() const noexcept;
		const T& value() const;

	private:
		http::StatusCode m_statusCode;
		T m_value;
	};

	http::StatusCode Reply::statusCode() const noexcept {
		return m_statusCode;
	}

	const T& Reply::value() const {
		return m_value;
	}
}

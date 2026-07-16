#pragma once

#include <optional>
#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
	template <typename T = void>
	class Reply {
	
	public:
		using ValueType = T;

		Reply(http::StatusCode statusCode, T value);
		explicit Reply(http::StatusCode statusCode);

		http::StatusCode statusCode() const noexcept;
		const T& value() const;
		bool hasValue() const noexcept;

	private:
		http::StatusCode m_statusCode;
		std::optional<T> m_value;
	};

	template <>
	class Reply<void>
	{
	public:
		using ValueType = void;

		explicit Reply(http::StatusCode statusCode)
			: m_statusCode(statusCode)
		{}

		http::StatusCode statusCode() const noexcept
		{
			return m_statusCode;
		}

		bool hasValue() const noexcept
		{
			return false;
		}

	private:
		http::StatusCode m_statusCode;
	};

	template <typename T>
	Reply<T>::Reply(http::StatusCode statusCode, T value) 
		: m_statusCode(statusCode),
		m_value(std::move(value))
	{ }

	template <typename T>
	Reply<T>::Reply(http::StatusCode statusCode)
		: m_statusCode(statusCode),
		m_value(std::nullopt)
	{ }

	template <typename T>
	http::StatusCode Reply<T>::statusCode() const noexcept {
		return m_statusCode;
	}

	template <typename T>
	const T& Reply<T>::value() const {
		return m_value.value();
	}

	template <typename T>
	bool Reply<T>::hasValue() const noexcept {
		return m_value.has_value();
	}
}

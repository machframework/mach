#pragma once

#include <optional>
#include <utility>

#include <mach/http/StatusCode.hpp>

namespace mach
{
    /**
     * Represents an HTTP response produced by a route handler.
     *
     * A reply contains an HTTP status code and, optionally, a response body.
     *
     * @tparam T The type of the response body. Use void for responses without a
     *           body.
     */
    template <typename T = void>
    class Reply {

    public:
        /// The type of the response body.
        using ValueType = T;

        /**
         * Initializes a reply with the specified status code and response body.
         *
         * @param statusCode The HTTP status code.
         * @param value The response body.
         */
        Reply(http::StatusCode statusCode, T value);

        /**
         * Initializes a reply with the specified status code and no response body.
         *
         * @param statusCode The HTTP status code.
         */
        explicit Reply(http::StatusCode statusCode);

        /**
         * Returns the HTTP status code.
         *
         * @return The HTTP status code.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]]
        http::StatusCode statusCode() const noexcept;

        /**
         * Returns the response body.
         *
         * @return A const reference to the response body.
         *
         * @throws std::bad_optional_access if the reply does not contain a value.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]]
        const T& value() const;

        /**
         * Determines whether the reply contains a response body.
         *
         * @return True if the reply contains a response body; otherwise, false.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]]
        bool hasValue() const noexcept;

    private:
        http::StatusCode m_statusCode;
        std::optional<T> m_value;
    };

    /**
     * Represents an HTTP response without a response body.
     */
    template <>
    class Reply<void> {
    public:
        /// The response body type.
        using ValueType = void;

        /**
         * Initializes a reply with the specified status code.
         *
         * @param statusCode The HTTP status code.
         */
        explicit Reply(http::StatusCode statusCode);

        /**
         * Returns the HTTP status code.
         *
         * @return The HTTP status code.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]]
        http::StatusCode statusCode() const noexcept;

        /**
         * Determines whether the reply contains a response body.
         *
         * @return Always false.
         *
         * @thread_safety This function is thread-safe.
         */
        [[nodiscard]]
        bool hasValue() const noexcept;

    private:
        http::StatusCode m_statusCode;
    };

    //==================================================
    // Implementation
    //==================================================

    template <typename T>
    Reply<T>::Reply(http::StatusCode statusCode, T value)
        : m_statusCode(statusCode), m_value(std::move(value)) {}

    template <typename T>
    Reply<T>::Reply(http::StatusCode statusCode) : m_statusCode(statusCode) {}

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

    inline Reply<void>::Reply(http::StatusCode statusCode) : m_statusCode(statusCode) {}

    inline http::StatusCode Reply<void>::statusCode() const noexcept {
        return m_statusCode;
    }

    inline bool Reply<void>::hasValue() const noexcept {
        return false;
    }
}

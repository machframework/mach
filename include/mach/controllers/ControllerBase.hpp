#pragma once

#include <string>

#include <mach/Context.hpp>
#include <mach/Reply.hpp>
#include <mach/results/Results.hpp>

namespace mach
{
    namespace detail::dispatching
    {
        template <typename TController, typename TResult, typename... TArgs>
        class ControllerActionInvoker;
    }

    /**
     * Base class for all HTTP controllers.
     *
     * Provides access to the current HTTP request and response through the
     * request() and response() member functions, and exposes convenience methods
     * for creating common HTTP responses, such as ok(), created(), badRequest(),
     * and notFound().
     *
     * Controller instances are created by the framework through the dependency
     * injection container and are intended to handle a single request.
     *
     * Thread safety:
     * - Not thread-safe. Controller instances must not be shared between threads.
     */
    class ControllerBase {

    public:
        /**
         * Returns the current HTTP request.
         *
         * @return A reference to the current request.
         *
         * The returned reference remains valid for the lifetime of the current
         * request being processed.
         *
         * @thread_safety This function is not thread-safe.
         */
        [[nodiscard]] mach::Request& request() noexcept;

        /**
         * Returns the current HTTP request.
         *
         * @return A const reference to the current request.
         *
         * The returned reference remains valid for the lifetime of the current
         * request being processed.
         *
         * @thread_safety This function is not thread-safe.
         */
        [[nodiscard]] const mach::Request& request() const noexcept;

        /**
         * Returns the current HTTP response.
         *
         * @return A reference to the current response.
         *
         * The returned reference remains valid for the lifetime of the current
         * request being processed.
         *
         * @thread_safety This function is not thread-safe.
         */
        [[nodiscard]] mach::Response& response() noexcept;

        /**
         * Returns the current HTTP response.
         *
         * @return A const reference to the current response.
         *
         * The returned reference remains valid for the lifetime of the current
         * request being processed.
         *
         * @thread_safety This function is not thread-safe.
         */
        [[nodiscard]] const mach::Response& response() const noexcept;

    protected:
        /**
         * Creates a 200 OK response containing the specified value.
         *
         * @tparam T The type of the response body.
         * 
         * @param value The response body.
         * 
         * @return A 200 OK reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> ok(T value);

        /**
         * Creates a 200 OK response containing the specified string.
         *
         * @param value The response body.
         * 
         * @return A 200 OK reply containing the specified string.
         */
        mach::Reply<std::string> ok(const char* value);

        /**
         * Creates a 200 OK response.
         * 
         * @return A 200 OK reply with no value.
         */
        mach::Reply<> ok();

        /**
         * Creates a 201 Created response containing the specified value.
         *
         * @tparam T The type of the response body.
         * 
         * @param value The response body.
         * 
         * @return A 201 Created reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> created(T value);

        /**
         * Creates a 201 Created response containing the specified string.
         *
         * @param value The response body.
         * 
         * @return A 201 Created reply containing the specified string.
         */
        mach::Reply<std::string> created(const char* value);

        /**
         * Creates a 201 Created response.
         *
         * @return A 201 Created reply with no value.
         */
        mach::Reply<> created();

        /**
         * Creates a 204 No Content response.
         *
         * @return A 204 No Content reply.
         */
        mach::Reply<> noContent();

        /**
         * Creates a 400 Bad Request response containing the specified value.
         *
         * @tparam T The type of the response body.
         * 
         * @param value The response body.
         * @return A 400 Bad Request reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> badRequest(T value);

        /**
         * Creates a 400 Bad Request response containing the specified string.
         *
         * @param value The response body.
         *
         * @return A 400 Bad Request reply containing the specified string.
         */
        mach::Reply<std::string> badRequest(const char* value);

        /**
         * Creates a 400 Bad Request response with no value.
         *
         * @tparam T The type of the response body. Defaults to void.
         *
         * @return A 400 Bad Request reply with no value.
         */
        template <typename T = void>
        Reply<T> badRequest() {
            return Reply<T>(http::StatusCode::BadRequest);
        }
        
        /**
         * Creates a 401 Unauthorized response containing the specified value.
         *
         * @tparam T The type of the response body.
         *
         * @param value The response body.
         * @return A 401 Unauthorized reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> unauthorized(T value);

        /**
         * Creates a 401 Unauthorized response containing the specified string.
         *
         * @param value The response body.
         *
         * @return A 401 Unauthorized reply containing the specified string.
         */
        mach::Reply<std::string> unauthorized(const char* value);

        /**
         * Creates a 401 Unauthorized response with no value.
         *
         * @tparam T The type of the response body. Defaults to void.
         *
         * @return A 401 Unauthorized reply with no value.
         */
        template <typename T = void>
        Reply<T> unauthorized() {
            return Reply<T>(http::StatusCode::Unauthorized);
        }

        /**
         * Creates a 403 Forbidden response containing the specified value.
         *
         * @tparam T The type of the response body.
         *
         * @param value The response body.
         * @return A 403 Forbidden reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> forbidden(T value);

        /**
         * Creates a 403 Forbidden response containing the specified string.
         *
         * @param value The response body.
         *
         * @return A 403 Forbidden reply containing the specified string.
         */
        mach::Reply<std::string> forbidden(const char* value);

        /**
         * Creates a 403 Forbidden response with no value.
         *
         * @tparam T The type of the response body. Defaults to void.
         *
         * @return A 403 Forbidden reply with no value.
         */
        template <typename T = void>
        Reply<T> forbidden() {
            return Reply<T>(http::StatusCode::Forbidden);
        }

        /**
         * Creates a 404 Not Found response containing the specified value.
         *
         * @tparam T The type of the response body.
         *
         * @param value The response body.
         * @return A 404 Not Found reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> notFound(T value);

        /**
         * Creates a 404 Not Found response containing the specified string.
         *
         * @param value The response body.
         *
         * @return A 404 Not Found reply containing the specified string.
         */
        mach::Reply<std::string> notFound(const char* value);

        /**
         * Creates a 404 Not Found response with no value.
         *
         * @tparam T The type of the response body. Defaults to void.
         *
         * @return A 404 Not Found reply with no value.
         */
        template <typename T = void>
        Reply<T> notFound() {
            return Reply<T>(http::StatusCode::NotFound);
        }

        /**
         * Creates a 409 Conflict response containing the specified value.
         *
         * @tparam T The type of the response body.
         *
         * @param value The response body.
         * @return A 409 Conflict reply containing the specified value.
         */
        template <typename T>
        mach::Reply<T> conflict(T value);

        /**
         * Creates a 409 Conflict response containing the specified string.
         *
         * @param value The response body.
         *
         * @return A 409 Conflict reply containing the specified string.
         */
        mach::Reply<std::string> conflict(const char* value);

        /**
         * Creates a 409 Conflict response with no value.
         *
         * @tparam T The type of the response body. Defaults to void.
         *
         * @return A 409 Conflict reply with no value.
         */
        template <typename T = void>
        Reply<T> conflict() {
            return Reply<T>(http::StatusCode::Conflict);
        }

    private:
        template <typename TController, typename TResult, typename... TArgs>
        friend class mach::detail::dispatching::ControllerActionInvoker;

        void setContext(mach::Context& context) noexcept;

        mach::Context* m_context = nullptr;
    };

    template <typename T>
    mach::Reply<T> ControllerBase::ok(T value) {
        return mach::ok<T>(value);
    }

    template <typename T>
    mach::Reply<T> ControllerBase::created(T value) {
        return mach::created<T>(value);
    }

    template <typename T>
    mach::Reply<T> ControllerBase::badRequest(T value) {
        return mach::badRequest<T>(value);
    }

    template <typename T>
    mach::Reply<T> ControllerBase::unauthorized(T value) {
        return mach::unauthorized<T>(value);
    }

    template <typename T>
    mach::Reply<T> ControllerBase::forbidden(T value) {
        return mach::forbidden<T>(value);
    }

    template <typename T>
    mach::Reply<T> ControllerBase::notFound(T value) {
        return mach::notFound<T>(value);
    }

    template <typename T>
    mach::Reply<T> ControllerBase::conflict(T value) {
        return mach::conflict<T>(value);
    }
}

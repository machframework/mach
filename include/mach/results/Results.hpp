#pragma once

#include <string>

#include <mach/Reply.hpp>
#include <mach/http/StatusCode.hpp>

namespace mach
{
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
    [[nodiscard]] Reply<T> ok(T value) {
        return Reply<T>(http::StatusCode::Ok, std::move(value));
    }

    /**
     * Creates a 200 OK response containing the specified string.
     *
     * @param value The response body.
     * 
     * @return A 200 OK reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> ok(const char* value);

    /**
     * Creates a 200 OK response.
     *
     * @return A 200 OK reply with no value.
     */
    [[nodiscard]] Reply<> ok();

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
    [[nodiscard]] Reply<T> created(T value) {
        return Reply<T>(http::StatusCode::Created, std::move(value));
    }

    /**
     * Creates a 201 Created response containing the specified string.
     *
     * @param value The response body.
     * 
     * @return A 201 Created reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> created(const char* value);

    /**
     * Creates a 201 Created response.
     *
     * @return A 201 Created reply with no value.
     */
    [[nodiscard]] mach::Reply<> created();

    /**
     * Creates a 204 No Content response.
     *
     * @return A 204 No Content reply.
     */
    [[nodiscard]] Reply<> noContent();

    /**
     * Creates a 400 Bad Request response containing the specified value.
     *
     * @tparam T The type of the response body.
     *
     * @param value The response body.
     *
     * @return A 400 Bad Request reply containing the specified value.
     */
    template <typename T>
    [[nodiscard]] Reply<T> badRequest(T value) {
        return Reply<T>(http::StatusCode::BadRequest, std::move(value));
    }

    /**
     * Creates a 400 Bad Request response containing the specified string.
     *
     * @param value The response body.
     *
     * @return A 400 Bad Request reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> badRequest(const char* value);

    /**
     * Creates a 400 Bad Request response with no value.
     *
     * @tparam T The type of the response body. Defaults to void.
     *
     * @return A 400 Bad Request reply with no value.
     */
    template <typename T = void>
    [[nodiscard]] Reply<T> badRequest() {
        return Reply<T>(http::StatusCode::BadRequest);
    }

    /**
     * Creates a 401 Unauthorized response containing the specified value.
     *
     * @tparam T The type of the response body.
     *
     * @param value The response body.
     *
     * @return A 401 Unauthorized reply containing the specified value.
     */
    template <typename T>
    [[nodiscard]] Reply<T> unauthorized(T value) {
        return Reply<T>(http::StatusCode::Unauthorized, std::move(value));
    }

    /**
     * Creates a 401 Unauthorized response containing the specified string.
     *
     * @param value The response body.
     *
     * @return A 401 Unauthorized reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> unauthorized(const char* value);

    /**
     * Creates a 401 Unauthorized response with no value.
     *
     * @tparam T The type of the response body. Defaults to void.
     *
     * @return A 401 Unauthorized reply with no value.
     */
    template <typename T = void>
    [[nodiscard]] Reply<T> unauthorized() {
        return Reply<T>(http::StatusCode::Unauthorized);
    }

    /**
     * Creates a 403 Forbidden response containing the specified value.
     *
     * @tparam T The type of the response body.
     *
     * @param value The response body.
     *
     * @return A 403 Forbidden reply containing the specified value.
     */
    template <typename T>
    [[nodiscard]] Reply<T> forbidden(T value) {
        return Reply<T>(http::StatusCode::Forbidden, std::move(value));
    }

    /**
     * Creates a 403 Forbidden response containing the specified string.
     *
     * @param value The response body.
     *
     * @return A 403 Forbidden reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> forbidden(const char* value);

    /**
     * Creates a 403 Forbidden response with no value.
     *
     * @tparam T The type of the response body. Defaults to void.
     *
     * @return A 403 Forbidden reply with no value.
     */
    template <typename T = void>
    [[nodiscard]] Reply<T> forbidden() {
        return Reply<T>(http::StatusCode::Forbidden);
    }

    /**
     * Creates a 404 Not Found response containing the specified value.
     *
     * @tparam T The type of the response body.
     *
     * @param value The response body.
     *
     * @return A 404 Not Found reply containing the specified value.
     */
    template <typename T>
    [[nodiscard]] Reply<T> notFound(T value) {
        return Reply<T>(http::StatusCode::NotFound, std::move(value));
    }

    /**
     * Creates a 404 Not Found response containing the specified string.
     *
     * @param value The response body.
     *
     * @return A 404 Not Found reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> notFound(const char* value);

    /**
     * Creates a 404 Not Found response with no value.
     *
     * @tparam T The type of the response body. Defaults to void.
     *
     * @return A 404 Not Found reply with no value.
     */
    template <typename T = void>
    [[nodiscard]] Reply<T> notFound() {
        return Reply<T>(http::StatusCode::NotFound);
    }

    /**
     * Creates a 409 Conflict response containing the specified value.
     *
     * @tparam T The type of the response body.
     *
     * @param value The response body.
     *
     * @return A 409 Conflict reply containing the specified value.
     */
    template <typename T>
    [[nodiscard]] Reply<T> conflict(T value) {
        return Reply<T>(http::StatusCode::Conflict, std::move(value));
    }

    /**
     * Creates a 409 Conflict response containing the specified string.
     *
     * @param value The response body.
     *
     * @return A 409 Conflict reply containing the specified string.
     */
    [[nodiscard]] Reply<std::string> conflict(const char* value);

    /**
     * Creates a 409 Conflict response with no value.
     *
     * @tparam T The type of the response body. Defaults to void.
     *
     * @return A 409 Conflict reply with no value.
     */
    template <typename T = void>
    [[nodiscard]] Reply<T> conflict() {
        return Reply<T>(http::StatusCode::Conflict);
    }
}

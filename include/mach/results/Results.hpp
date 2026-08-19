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
    Reply<T> ok(T value) {
        return Reply<T>(http::StatusCode::Ok, std::move(value));
    }

    /**
     * Creates a 200 OK response containing the specified string.
     *
     * @param value The response body.
     * 
     * @return A 200 OK reply containing the specified string.
     */
    Reply<std::string> ok(const char* value);

    /**
     * Creates a 200 OK response.
     *
     * @return A 200 OK reply with no value.
     */
    Reply<> ok();

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
    Reply<T> created(T value) {
        return Reply<T>(http::StatusCode::Created, std::move(value));
    }

    /**
     * Creates a 201 Created response containing the specified string.
     *
     * @param value The response body.
     * 
     * @return A 201 Created reply containing the specified string.
     */
    Reply<std::string> created(const char* value);

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
    Reply<> noContent();

    /**
     * Creates a 400 Bad Request response.
     *
     * @tparam T The type of the response body. Defaults to void.
     * @return A 400 Bad Request reply.
     */
    template <typename T = void>
    Reply<T> badRequest() {
        return Reply<T>(http::StatusCode::BadRequest);
    }

    /**
     * Creates a 401 Unauthorized response.
     *
     * @tparam T The type of the response body. Defaults to void.
     * @return A 401 Unauthorized reply.
     */
    template <typename T = void>
    Reply<T> unauthorized() {
        return Reply<T>(http::StatusCode::Unauthorized);
    }

    /**
     * Creates a 403 Forbidden response.
     *
     * @tparam T The type of the response body. Defaults to void.
     * @return A 403 Forbidden reply.
     */
    template <typename T = void>
    Reply<T> forbidden() {
        return Reply<T>(http::StatusCode::Forbidden);
    }

    /**
     * Creates a 404 Not Found response.
     *
     * @tparam T The type of the response body. Defaults to void.
     * @return A 404 Not Found reply.
     */
    template <typename T = void>
    Reply<T> notFound() {
        return Reply<T>(http::StatusCode::NotFound);
    }

    /**
     * Creates a 409 Conflict response.
     *
     * @tparam T The type of the response body. Defaults to void.
     * @return A 409 Conflict reply.
     */
    template <typename T = void>
    Reply<T> conflict() {
        return Reply<T>(http::StatusCode::Conflict);
    }
}

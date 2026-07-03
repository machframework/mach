#pragma once

#include <string>

#include <mach/http/StatusCode.hpp>
#include <mach/results/Reply.hpp>

namespace mach::results {

    template <typename T>
    Reply<T> ok(T value) {
        return Reply<T>(http::StatusCode::Ok, std::move(value));
    }

    inline Reply<std::string> ok(const char* value) {
        return ok(std::string(value));
    }

    template <typename T>
    Reply<T> created(T value) {
        return Reply<T>(http::StatusCode::Created, std::move(value));
    }

    inline Reply<std::string> created(const char* value) {
        return created(std::string(value));
    }

    template <typename T>
    Reply<T> noContent() {
        return Reply<T>(http::StatusCode::NoContent);
    }

    template <typename T>
    Reply<T> badRequest() {
        return Reply<T>(http::StatusCode::BadRequest);
    }

    template <typename T>
    Reply<T> unauthorized() {
        return Reply<T>(http::StatusCode::Unauthorized);
    }

    template <typename T>
    Reply<T> forbidden() {
        return Reply<T>(http::StatusCode::Forbidden);
    }

    template <typename T>
    Reply<T> notFound() {
        return Reply<T>(http::StatusCode::NotFound);
    }

    template <typename T>
    Reply<T> conflict() {
        return Reply<T>(http::StatusCode::Conflict);
    }

}

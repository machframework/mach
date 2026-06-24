#pragma once

#include <string>

#include <mach/http/StatusCode.hpp>
#include <mach/results/Reply.hpp>

namespace mach::results
{
	class Results {

	public:
		template <typename T>
		static mach::Reply<T> ok(T value);

		static mach::Reply<std::string> ok(const char* value);

		template <typename T>
		static mach::Reply<T> created(T value);

		static mach::Reply<std::string> created(const char* value);

		template <typename T>
		static mach::Reply<T> noContent();

		template <typename T>
		static mach::Reply<T> badRequest();

		template <typename T>
		static mach::Reply<T> unauthorized();

		template <typename T>
		static mach::Reply<T> forbidden();

		template <typename T>
		static mach::Reply<T> notFound();

		template <typename T>
		static mach::Reply<T> conflict();
	};

	template <typename T>
	mach::Reply<T> Results::ok(T value) {
		return mach::Reply<T>(http::StatusCode::Ok, value);
	}

	inline mach::Reply<std::string> Results::ok(const char* value) {
		return ok<std::string>(value);
	}

	template <typename T>
	mach::Reply<T> Results::created(T value) {
		return mach::Reply<T>(http::StatusCode::Created, value);
	}

	inline mach::Reply<std::string> Results::created(const char* value) {
		return created<std::string>(value);
	}

	template <typename T>
	mach::Reply<T> Results::noContent() {
		return mach::Reply<T>(http::StatusCode::NoContent);
	}

	template <typename T>
	mach::Reply<T> Results::badRequest() {
		return mach::Reply<T>(http::StatusCode::BadRequest);
	}

	template <typename T>
	mach::Reply<T> Results::unauthorized() {
		return mach::Reply<T>(http::StatusCode::Unauthorized);
	}

	template <typename T>
	mach::Reply<T> Results::forbidden() {
		return mach::Reply<T>(http::StatusCode::Forbidden);
	}

	template <typename T>
	mach::Reply<T> Results::notFound() {
		return mach::Reply<T>(http::StatusCode::NotFound);
	}

	template <typename T>
	mach::Reply<T> Results::conflict() {
		return mach::Reply<T>(http::StatusCode::Conflict);
	}
}

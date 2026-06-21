#pragma once

#include <string>

#include <mach/http/StatusCode.hpp>
#include <mach/results/Reply.hpp>

namespace mach::results
{
	class Results {
		template <typename T>
		static mach::Reply<T> ok(T value);

		template <typename T>
		static mach::Reply<T> created(T value);

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

	template <typename T>
	mach::Reply<T> Results::created(T value) {
		return mach::Reply<T>(http::StatusCode::Created, value);
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

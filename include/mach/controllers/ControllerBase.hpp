#pragma once

#include <string>

#include <mach/Context.hpp>
#include <mach/results/Results.hpp>
#include <mach/results/Reply.hpp>

namespace mach
{
	class ControllerBase {

	public:
		mach::Context* context = nullptr;

	protected:
		template <typename T>
		mach::Reply<T> ok(T value);

		mach::Reply<std::string> ok(const char* value);

		template <typename T>
		mach::Reply<T> created(T value);

		mach::Reply<std::string> created(const char* value);

		mach::Reply<> noContent();

		template <typename T = void>
		mach::Reply<T> badRequest();

		template <typename T = void>
		mach::Reply<T> unauthorized();

		template <typename T = void>
		mach::Reply<T> forbidden();

		template <typename T = void>
		mach::Reply<T> notFound();

		template <typename T = void>
		mach::Reply<T> conflict();
	};

	template <typename T>
	mach::Reply<T> ControllerBase::ok(T value) {
		return mach::results::ok<T>(value);
	}

	inline mach::Reply<std::string> ControllerBase::ok(const char* value) {
		return mach::results::ok(value);
	}

	template <typename T>
	mach::Reply<T> ControllerBase::created(T value) {
		return mach::results::created<T>(value);
	}

	inline mach::Reply<std::string> ControllerBase::created(const char* value) {
		return mach::results::created(value);
	}

	inline mach::Reply<> ControllerBase::noContent() {
		return mach::results::noContent();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::badRequest() {
		return mach::results::badRequest<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::unauthorized() {
		return mach::results::unauthorized<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::forbidden() {
		return mach::results::forbidden<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::notFound() {
		return mach::results::notFound<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::conflict() {
		return mach::results::conflict<T>();
	}
}

#pragma once

#include <mach/Context.hpp>
#include <mach/results/Results.hpp>
#include <mach/results/Reply.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

namespace mach
{
	class ControllerBase {

	public:
		mach::Context* context = nullptr;

	protected:

		template <typename T>
		mach::Reply<T> ok(T value);

		template <typename T>
		mach::Reply<T> created(T value);

		template <typename T>
		mach::Reply<T> noContent();

		template <typename T>
		mach::Reply<T> badRequest();

		template <typename T>
		mach::Reply<T> unauthorized();

		template <typename T>
		mach::Reply<T> forbidden();

		template <typename T>
		mach::Reply<T> notFound();

		template <typename T>
		mach::Reply<T> conflict();
	};

	template <typename T>
	mach::Reply<T> ControllerBase::ok(T value) {
		return mach::results::Results::ok<T>(value);
	}

	template <typename T>
	mach::Reply<T> ControllerBase::created(T value) {
		return mach::results::Results::created<T>(value);
	}

	template <typename T>
	mach::Reply<T> ControllerBase::noContent() {
		return mach::results::Results::noContent<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::badRequest() {
		return mach::results::Results::badRequest<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::unauthorized() {
		return mach::results::Results::unauthorized<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::forbidden() {
		return mach::results::Results::forbidden<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::notFound() {
		return mach::results::Results::notFound<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::conflict() {
		return mach::results::Results::conflict<T>();
	}
}

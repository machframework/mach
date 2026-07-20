#pragma once

#include <string>

#include <mach/Context.hpp>
#include <mach/results/Results.hpp>
#include <mach/results/Reply.hpp>

namespace mach
{
	namespace detail::dispatching
	{
		template <
			typename TController,
			typename TResult,
			typename... TArgs
		>
		class ControllerActionInvoker;
	}

	class ControllerBase {

	public:
		[[nodiscard]]
		mach::Request& request() noexcept;

		[[nodiscard]]
		const mach::Request& request() const noexcept;

		[[nodiscard]]
		mach::Response& response() noexcept;

		[[nodiscard]]
		const mach::Response& response() const noexcept;

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

	private:
		template <
			typename TController,
			typename TResult,
			typename... TArgs
		>
		friend class mach::detail::dispatching::ControllerActionInvoker;

		void setContext(mach::Context& context) noexcept {
			m_context = &context;
		}

		mach::Context* m_context = nullptr;
	};

	template <typename T>
	mach::Reply<T> ControllerBase::ok(T value) {
		return mach::ok<T>(value);
	}

	inline mach::Reply<std::string> ControllerBase::ok(const char* value) {
		return mach::ok(value);
	}

	template <typename T>
	mach::Reply<T> ControllerBase::created(T value) {
		return mach::created<T>(value);
	}

	inline mach::Reply<std::string> ControllerBase::created(const char* value) {
		return mach::created(value);
	}

	inline mach::Reply<> ControllerBase::noContent() {
		return mach::noContent();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::badRequest() {
		return mach::badRequest<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::unauthorized() {
		return mach::unauthorized<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::forbidden() {
		return mach::forbidden<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::notFound() {
		return mach::notFound<T>();
	}

	template <typename T>
	mach::Reply<T> ControllerBase::conflict() {
		return mach::conflict<T>();
	}
}

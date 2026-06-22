#pragma once

#include <string_view>
#include <vector>

#include <mach/http/Method.hpp>

#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/dispatching/ControllerActionDescriptor.hpp>
#include <mach/detail/core/FunctionTraits.hpp>

namespace mach
{
	template <typename TController>
	class ControllerBuilder {
		
	public:
		ControllerBuilder(const ControllerBuilder&) = delete;
		ControllerBuilder& operator=(const ControllerBuilder&) = delete;

		ControllerBuilder(ControllerBuilder&&) = delete;
		ControllerBuilder& operator=(ControllerBuilder&&) = delete;

		template <typename THandler>
		ControllerBuilder& get(std::string_view pattern, THandler handler);

	private:
		explicit ControllerBuilder();

		template <typename THandler>
		void addControllerMethod(http::Method method, std::string_view pattern, THandler handler);

		std::vector<detail::routing::Endpoint> m_controllerEndpoints;

		friend class AppBuilder;
	};

	template <typename TController>
	template <typename THandler>
	ControllerBuilder<TController>& ControllerBuilder<TController>::get(
		std::string_view pattern,
		THandler handler
	) {
		addControllerMethod(http::Method::Get, path, std::forward(handler));
		return *this;
	}

	template <typename TController>
	template <typename THandler>
	void ControllerBuilder<TController>::addControllerMethod(
		http::Method method,
		std::string_view pattern,
		THandler handler
	) {
		using Traits = detail::FunctionTraits<Handler>;

		using HandlerControllerType = typename Traits::ClassType;
		using ReturnType = typename Traits::ReturnType;
		using ArgsTuple = typename Traits::ArgsTuple;

		static_assert(
			std::same_as<HandlerControllerType, TController>,
			"Mach error: route handler must belong to the controller being registered."
		);

		detail::routing::RouteEndpoint{
			.method = method,
			.pattern = std::string(pattern),
			.kind = EndpointKind::ControllerAction,
			.controllerAction = std::make_unique<ControllerActionInvoker<UserController>>(&handler)
		};
	}
}

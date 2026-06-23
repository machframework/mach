#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include <mach/detail/controllers/ControllerTraits.hpp>
#include <mach/detail/core/FunctionTraits.hpp>
#include <mach/detail/dispatching/ControllerActionDescriptor.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/http/Method.hpp>

#include <mach/detail/routing/Router.hpp>

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
        ControllerBuilder<TController>& get(std::string_view pattern, THandler&& handler);

    private:
        explicit ControllerBuilder();

        template <typename THandler>
        void addControllerMethod(http::Method method, std::string_view pattern, THandler&& handler);

        std::string m_route;
        std::vector<detail::routing::RouteEndpoint> m_controllerEndpoints;

        friend class App;
        friend class AppBuilder;
    };

    template <typename TController>
    ControllerBuilder<TController>::ControllerBuilder()
    {
        static_assert(
            detail::controllers::MachController<TController>,
            "Mach error: TController must be a valid Mach controller."
        );

        m_route = TController::route;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::get(
        std::string_view pattern,
        THandler&& handler)
    {
        addControllerMethod(
            http::Method::Get,
            pattern,
            std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    void ControllerBuilder<TController>::addControllerMethod(
        http::Method method,
        std::string_view pattern,
        THandler&& handler)
    {
        using HandlerType = std::remove_cvref_t<THandler>;
        using Traits = detail::FunctionTraits<HandlerType>;

        using HandlerControllerType = typename Traits::ClassType;
        using ReturnType = typename Traits::ReturnType;
        using ArgsTuple = typename Traits::ArgsTuple;

        static_assert(
            std::same_as<HandlerControllerType, TController>,
            "Mach error: route handler must belong to the controller being registered."
            );

        detail::routing::RouteEndpoint endpoint{
            .method = method,
            .pattern = m_route + std::string(pattern),
            .kind = detail::routing::EndpointKind::ControllerAction,
            .controllerAction =
                std::make_unique<detail::dispatching::ControllerActionInvoker<TController>>(
                    std::forward<THandler>(handler))
        };

        m_controllerEndpoints.emplace_back(std::move(endpoint));
    }
}

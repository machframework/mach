#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include <mach/detail/core/FunctionTraits.hpp>
#include <mach/detail/dispatching/ControllerActionInvoker.hpp>
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
        ControllerBuilder<TController>& mapGet(std::string_view pattern, THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapGet(THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapPost(std::string_view pattern, THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapPost(THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapPut(std::string_view pattern, THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapPut(THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapPatch(std::string_view pattern, THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapPatch(THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapDelete(std::string_view pattern, THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapDelete(THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapHead(std::string_view pattern, THandler&& handler);

        template <typename THandler>
        ControllerBuilder<TController>& mapHead(THandler&& handler);

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
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapGet(
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
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapGet(THandler&& handler)
    {
        return mapGet<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPost(
        std::string_view pattern,
        THandler&& handler)
    {
        addControllerMethod(
            http::Method::Post,
            pattern,
            std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPost(THandler&& handler)
    {
        return mapPost<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPut(
        std::string_view pattern,
        THandler&& handler)
    {
        addControllerMethod(
            http::Method::Put,
            pattern,
            std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPut(THandler&& handler)
    {
        return mapPut<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPatch(
        std::string_view pattern,
        THandler&& handler)
    {
        addControllerMethod(
            http::Method::Patch,
            pattern,
            std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPatch(THandler&& handler)
    {
        return mapPatch<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapDelete(
        std::string_view pattern,
        THandler&& handler)
    {
        addControllerMethod(
            http::Method::Delete,
            pattern,
            std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapDelete(THandler&& handler)
    {
        return mapDelete<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapHead(
        std::string_view pattern,
        THandler&& handler)
    {
        addControllerMethod(
            http::Method::Head,
            pattern,
            std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapHead(THandler&& handler)
    {
        return mapHead<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    void ControllerBuilder<TController>::addControllerMethod(
        http::Method method,
        std::string_view pattern,
        THandler&& handler)
    {
        using HandlerType = std::remove_cvref_t<THandler>;

        constexpr bool isMemberFunction =
            std::is_member_function_pointer_v<HandlerType>;

        static_assert(
            isMemberFunction,
            "Mach error: route handler must be a non-static controller member function."
            );

        if constexpr (isMemberFunction) {
            using Traits = detail::FunctionTraits<HandlerType>;

            using HandlerControllerType = typename Traits::ClassType;
            using ReturnType = typename Traits::ReturnType;
            using ArgsTuple = typename Traits::ArgsTuple;

			constexpr bool isSameController =
				std::same_as<HandlerControllerType, TController>;

            static_assert(
                isSameController,
                "Mach error: route handler must belong to the controller being registered."
                );

            if constexpr (isSameController) {
                using InvokerType =
                    typename detail::dispatching::ControllerActionInvokerFromTuple<
                    TController,
                    ReturnType,
                    ArgsTuple
                    >::Type;

                static_assert(
                    std::same_as<HandlerControllerType, TController>,
                    "Mach error: route handler must belong to the controller being registered."
                    );

				if (!pattern.empty() && pattern.front() != '/') {
					throw std::invalid_argument(
						"Mach error: route pattern must start with a leading slash ('/')."
					);
				}

                detail::routing::RouteEndpoint endpoint{
                    .method = method,
                    .pattern = m_route + std::string(pattern),
                    .invoker = std::make_unique<InvokerType>(
                        std::forward<THandler>(handler)
                     )
                };

                m_controllerEndpoints.emplace_back(std::move(endpoint));
            }
        }
    }
}

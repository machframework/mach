#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <mach/Context.hpp>

#include <mach/detail/core/FunctionTraits.hpp>
#include <mach/detail/core/TupleTraits.hpp>
#include <mach/detail/dispatching/ControllerActionInvoker.hpp>
#include <mach/detail/dispatching/ReplyTraits.hpp>
#include <mach/detail/routing/RouteEndpoint.hpp>
#include <mach/detail/routing/Router.hpp>
#include <mach/http/Method.hpp>

namespace mach
{
    /**
     * Configures routes for a controller.
     *
     * Used to associate HTTP routes with member functions of a controller before
     * the application is built.
     *
     * ControllerBuilder is obtained from AppBuilder and is not intended to be
     * instantiated directly.
     *
     * @tparam TController The controller type being configured.
     *
     * Thread safety:
     * - Not thread-safe. Controller configuration should be performed from one thread.
     */
    template <typename TController>
    class ControllerBuilder {

    public:
        ControllerBuilder(const ControllerBuilder&) = delete;
        ControllerBuilder& operator=(const ControllerBuilder&) = delete;

        ControllerBuilder(ControllerBuilder&&) = delete;
        ControllerBuilder& operator=(ControllerBuilder&&) = delete;

        /**
         * Maps an HTTP GET route to a controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param pattern The route pattern, relative to the controller's base route.
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapGet(std::string_view pattern, THandler&& handler);

        /**
         * Maps the controller's base route to an HTTP GET controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapGet(THandler&& handler);

        /**
         * Maps an HTTP POST route to a controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param pattern The route pattern, relative to the controller's base route.
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapPost(std::string_view pattern, THandler&& handler);

        /**
         * Maps the controller's base route to an HTTP POST controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapPost(THandler&& handler);

        /**
         * Maps an HTTP PUT route to a controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param pattern The route pattern, relative to the controller's base route.
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapPut(std::string_view pattern, THandler&& handler);

        /**
         * Maps the controller's base route to an HTTP PUT controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapPut(THandler&& handler);

        /**
         * Maps an HTTP PATCH route to a controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param pattern The route pattern, relative to the controller's base route.
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapPatch(std::string_view pattern, THandler&& handler);

        /**
         * Maps the controller's base route to an HTTP PATCH controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapPatch(THandler&& handler);

        /**
         * Maps an HTTP DELETE route to a controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param pattern The route pattern, relative to the controller's base route.
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapDelete(std::string_view pattern, THandler&& handler);

        /**
         * Maps the controller's base route to an HTTP DELETE controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapDelete(THandler&& handler);

        /**
         * Maps an HTTP HEAD route to a controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param pattern The route pattern, relative to the controller's base route.
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
        template <typename THandler>
        ControllerBuilder<TController>& mapHead(std::string_view pattern, THandler&& handler);

        /**
         * Maps the controller's base route to an HTTP HEAD controller action.
         *
         * @tparam THandler The type of the controller action.
         * 
         * @param handler The controller action to invoke.
         * 
         * @throws std::logic_error If a route with the same method and pattern
         *         has already been registered.
         * 
         * @return A reference to this builder.
         */
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
    ControllerBuilder<TController>::ControllerBuilder() {
        static_assert(
            detail::controllers::MachController<TController>,
            "Mach error: TController must be a valid Mach controller.");

        m_route = TController::route;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapGet(
        std::string_view pattern,
        THandler&& handler) {
        addControllerMethod(http::Method::Get, pattern, std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapGet(THandler&& handler) {
        return mapGet<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPost(
        std::string_view pattern,
        THandler&& handler) {
        addControllerMethod(http::Method::Post, pattern, std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPost(THandler&& handler) {
        return mapPost<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPut(
        std::string_view pattern,
        THandler&& handler) {
        addControllerMethod(http::Method::Put, pattern, std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPut(THandler&& handler) {
        return mapPut<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPatch(
        std::string_view pattern,
        THandler&& handler) {
        addControllerMethod(http::Method::Patch, pattern, std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapPatch(THandler&& handler) {
        return mapPatch<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapDelete(
        std::string_view pattern,
        THandler&& handler) {
        addControllerMethod(http::Method::Delete, pattern, std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapDelete(THandler&& handler) {
        return mapDelete<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapHead(
        std::string_view pattern,
        THandler&& handler) {
        addControllerMethod(http::Method::Head, pattern, std::forward<THandler>(handler));

        return *this;
    }

    template <typename TController>
    template <typename THandler>
    ControllerBuilder<TController>& ControllerBuilder<TController>::mapHead(THandler&& handler) {
        return mapHead<THandler>("", std::forward<THandler>(handler));
    }

    template <typename TController>
    template <typename THandler>
    void ControllerBuilder<TController>::addControllerMethod(
        http::Method method,
        std::string_view pattern,
        THandler&& handler) {
        using HandlerType = std::remove_cvref_t<THandler>;

        constexpr bool isMemberFunction = std::is_member_function_pointer_v<HandlerType>;

        static_assert(
            isMemberFunction,
            "Mach error: route handler must be a non-static controller member function.");

        if constexpr (isMemberFunction) {
            using Traits = detail::traits::FunctionTraits<HandlerType>;

            using HandlerControllerType = Traits::ClassType;
            using ReturnType = Traits::ReturnType;
            using ArgsTuple = Traits::ArgsTuple;

            constexpr bool containsContextArg =
                mach::detail::traits::tuple_contains_v<mach::Context, ArgsTuple>;

            constexpr bool isSameController = std::same_as<HandlerControllerType, TController>;

            static_assert(
                isSameController,
                "Mach error: route handler must belong to the controller being registered.");

            static_assert(
                !containsContextArg,
                "Mach error: controller actions must not accept a Context parameter. "
                "Use the inherited 'context' member instead.");

            if constexpr (isSameController) {
                using InvokerType = detail::dispatching::
                    ControllerActionInvokerFromTuple<TController, ReturnType, ArgsTuple>::Type;

                constexpr bool isReply = detail::traits::dispatching::is_reply_v<ReturnType>;

                static_assert(
                    isReply,
                    "Mach error: controller actions must return mach::Reply<T>.");

                if constexpr (isReply) {
                    if (!pattern.empty() && pattern.front() != '/') {
                        throw std::invalid_argument(
                            "Mach error: route pattern must start with a leading slash ('/').");
                    }

                    detail::routing::RouteEndpoint endpoint{
                        .method = method,
                        .pattern = m_route + std::string(pattern),
                        .invoker = std::make_unique<InvokerType>(std::forward<THandler>(handler))};

                    m_controllerEndpoints.emplace_back(std::move(endpoint));
                }
            }
        }
    }
}

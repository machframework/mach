#pragma once

#include <functional>

namespace mach::detail::middleware
{
    template <typename TMiddleware>
    class MiddlewareInvoker;
}

namespace mach
{
    /**
     * Represents the next step in the middleware pipeline.
     *
     * Calling next() transfers execution to the next middleware or endpoint
     * in the request pipeline. A Next instance may only be invoked once.
     */
    class Next {

    public:
        Next(const Next&) = delete;
        Next& operator=(const Next&) = delete;

        /**
         * Continues execution of the middleware pipeline.
         *
         * This function may only be called once. Calling it more than once
         * results in a std::logic_error being thrown.
         *
         * @throws std::logic_error If next() has already been invoked.
         */
        void operator()();

    private:
        explicit Next(std::function<void()> next);

        std::function<void()> m_next;
        bool m_invoked = false;

        template <typename TMiddleware>
        friend class detail::middleware::MiddlewareInvoker;
    };
}

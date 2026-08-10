#pragma once

namespace mach
{
    struct Context;
    class Next;
    class Logger;
}

namespace mach::detail::exceptions
{
    class ExceptionMiddleware {

    public:
        explicit ExceptionMiddleware(Logger& logger);

        void invoke(mach::Context& context, mach::Next& next);

    private:
        Logger& m_logger;
    };
}

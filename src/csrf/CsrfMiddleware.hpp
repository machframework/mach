#pragma once

#include <string>

namespace mach
{
    struct Context;
    class Next;
}

namespace mach::detail::csrf
{
    class CsrfMiddleware {

    public:
        explicit CsrfMiddleware();

        void invoke(mach::Context& context, mach::Next& next);

    private:
        std::string generateToken() const;
    };
}

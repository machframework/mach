#pragma once

#include <string>

#include <mach/detail/csrf/CsrfOptions.hpp>

namespace mach
{
    struct Context;
    class Next;
}

namespace mach::detail::csrf
{
    class CsrfMiddleware {

    public:
        explicit CsrfMiddleware(CsrfOptions& options);

        void invoke(mach::Context& context, mach::Next& next);

    private:
        std::string generateToken() const;

        CsrfOptions m_options;
    };
}

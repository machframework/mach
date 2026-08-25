#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/middleware/Next.hpp>

#ifndef MACH_MIDDLEWARE_COMPILE_FAILURE_TEST
#define MACH_MIDDLEWARE_COMPILE_FAILURE_TEST 0
#endif

namespace
{
    // -------------------------------------------------------------------------
    // Valid middleware types
    // -------------------------------------------------------------------------

    class ValidMiddleware {
    public:
        void invoke(mach::Context&, mach::Next& next) {
            next();
        }
    };

    class Dependency {};

    class MiddlewareWithDependency {
    public:
        explicit MiddlewareWithDependency(Dependency&) {}

        void invoke(mach::Context&, mach::Next& next) {
            next();
        }
    };

    // -------------------------------------------------------------------------
    // Invalid middleware types
    // -------------------------------------------------------------------------

    using NonClassMiddleware = int;

    class MissingInvokeMiddleware {};

    class NonVoidInvokeMiddleware {
    public:
        int invoke(mach::Context&, mach::Next&) {
            return 0;
        }
    };

    class ContextByValueMiddleware {
    public:
        void invoke(mach::Context, mach::Next&) {}
    };

    class ConstContextMiddleware {
    public:
        void invoke(const mach::Context&, mach::Next&) {}
    };

    class NextByValueMiddleware {
    public:
        void invoke(mach::Context&, mach::Next) {}
    };

    class ConstNextMiddleware {
    public:
        void invoke(mach::Context&, const mach::Next&) {}
    };

    class ExtraParameterMiddleware {
    public:
        void invoke(mach::Context&, mach::Next&, int) {}
    };

    class WrongOverloadedMiddleware {
    public:
        void invoke(mach::Context&) {}

        void invoke(mach::Context&, const mach::Next&) {}
    };

    class ControllerMiddleware final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/firewall";

        void invoke(mach::Context&, mach::Next&) {}
    };

    void instantiateValidRegistrations() {
        {
            auto builder = mach::AppBuilder();

            builder.use<ValidMiddleware>();
        }

        {
            auto builder = mach::AppBuilder();

            builder.addScoped<Dependency>();
            builder.use<MiddlewareWithDependency, Dependency>();
        }
    }

    void instantiateExpectedCompileFailure() {
        auto builder = mach::AppBuilder();

#if MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 1

        builder.use<NonClassMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 2

        builder.use<MissingInvokeMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 3

        builder.use<NonVoidInvokeMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 4

        builder.use<ContextByValueMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 5

        builder.use<ConstContextMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 6

        builder.use<NextByValueMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 7

        builder.use<ConstNextMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 8

        builder.use<ExtraParameterMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 9

        builder.use<WrongOverloadedMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 10

        builder.use<ControllerMiddleware>();

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST != 0

#error Unknown MACH_MIDDLEWARE_COMPILE_FAILURE_TEST value

#endif
    }
}

int main() {
#if MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 0
    instantiateValidRegistrations();
#else
    instantiateExpectedCompileFailure();
#endif

    return 0;
}

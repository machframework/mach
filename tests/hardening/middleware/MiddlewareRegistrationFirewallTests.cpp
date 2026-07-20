#include <string_view>
#include <utility>

#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/middleware/Next.hpp>

#include "Testing.hpp"

// ============================================================
// Valid middleware types
// ============================================================

class ValidMiddleware {

public:
    void invoke(mach::Context&, mach::Next& next) {
        next();
    }
};

class Dependency {};

class MiddlewareWithDependency {

public:
    explicit MiddlewareWithDependency(Dependency&)
    {}

    void invoke(mach::Context&, mach::Next& next) {
        next();
    }
};

// ============================================================
// Invalid middleware types
// ============================================================

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
    void invoke(mach::Context, mach::Next&)
    {}
};

class ConstContextMiddleware {

public:
    void invoke(const mach::Context&, mach::Next&)
    {}
};

class NextByValueMiddleware {

public:
    void invoke(mach::Context&, mach::Next)
    {}
};

class ConstNextMiddleware {

public:
    void invoke(mach::Context&, const mach::Next&)
    {}
};

class ExtraParameterMiddleware {

public:
    void invoke(mach::Context&, mach::Next&, int)
    {}
};

class WrongOverloadedMiddleware {

public:
    void invoke(mach::Context&)
    {}

    void invoke(mach::Context&, const mach::Next&)
    {}
};

class ControllerMiddleware final : public mach::ControllerBase {

public:
    static inline constexpr std::string_view route = "/firewall";

    void invoke(mach::Context&, mach::Next&)
    {}
};

// ============================================================
// Test selector
// ============================================================
//
// 0  - valid middleware
// 1  - valid middleware with constructor dependency
// 2  - non-class middleware
// 3  - missing invoke()
// 4  - non-void invoke()
// 5  - Context by value
// 6  - const Context&
// 7  - Next by value
// 8  - const Next&
// 9  - extra invoke() parameter
// 10 - invalid overloaded invoke()
// 11 - controller registered as middleware
//
// ============================================================

#ifndef MACH_MIDDLEWARE_COMPILE_FAILURE_TEST
#define MACH_MIDDLEWARE_COMPILE_FAILURE_TEST 0
#endif

int main() {
    auto builder =
        mach::AppBuilder(std::move(testing::serverOptions));

#if MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 0

    builder.use<ValidMiddleware>();

    testing::success(
        "Valid middleware registration passed!"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 1

    builder.addScoped<Dependency>();
    builder.use<MiddlewareWithDependency, Dependency>();

    testing::success(
        "Middleware constructor injection registration passed!"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 2

    builder.use<NonClassMiddleware>();

    testing::fail(
        "Non-class middleware registration",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 3

    builder.use<MissingInvokeMiddleware>();

    testing::fail(
        "Middleware without invoke()",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 4

    builder.use<NonVoidInvokeMiddleware>();

    testing::fail(
        "Middleware with a non-void invoke()",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 5

    builder.use<ContextByValueMiddleware>();

    testing::fail(
        "Middleware accepting Context by value",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 6

    builder.use<ConstContextMiddleware>();

    testing::fail(
        "Middleware accepting const Context&",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 7

    builder.use<NextByValueMiddleware>();

    testing::fail(
        "Middleware accepting Next by value",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 8

    builder.use<ConstNextMiddleware>();

    testing::fail(
        "Middleware accepting const Next&",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 9

    builder.use<ExtraParameterMiddleware>();

    testing::fail(
        "Middleware invoke() with an extra parameter",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 10

    builder.use<WrongOverloadedMiddleware>();

    testing::fail(
        "Middleware with invalid invoke() overloads",
        "Should've been rejected"
    );

#elif MACH_MIDDLEWARE_COMPILE_FAILURE_TEST == 11

    builder.use<ControllerMiddleware>();

    testing::fail(
        "Controller registration as middleware",
        "Should've been rejected"
    );

#else

#error Unknown MACH_MIDDLEWARE_COMPILE_FAILURE_TEST value.

#endif
    return 0;
}

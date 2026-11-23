
// Compile-failure tests for Minimal API handler signatures.
// Compile separately with -DMACH_COMPILE_FAILURE_CASE=<1..9>.
// Every selected case must fail to compile for the expected Mach diagnostic.

#include <string>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/Reply.hpp>
#include <mach/binding/Body.hpp>
#include <mach/binding/Param.hpp>
#include <mach/binding/Query.hpp>

#ifndef MACH_COMPILE_FAILURE_CASE
#error "MACH_COMPILE_FAILURE_CASE must be defined"
#endif

#if MACH_COMPILE_FAILURE_CASE < 0 || MACH_COMPILE_FAILURE_CASE > 9
#error "MACH_COMPILE_FAILURE_CASE must be between 0 and 9"
#endif

int main() {
    auto app = mach::AppBuilder().build();

#if MACH_COMPILE_FAILURE_CASE == 0
    app.mapGet("/users/{id}", [](mach::Param<int, "id"> id) {
        return mach::ok();
    });

    app.mapGet(
        "/users/{id}/posts",
        [](mach::Context& context, mach::Param<int, "id"> id, mach::Query<int, "page"> page) {
            return mach::ok();
        });

    app.mapPost("/users", [](mach::Body<std::string> body) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 1
    // Expected: binding wrappers must be passed by value.
    app.mapGet("/users/{id}", [](mach::Param<int, "id">& id) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 2
    // Expected: const reference to a binding wrapper is rejected.
    app.mapGet("/users/{id}", [](const mach::Param<int, "id">& id) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 3
    // Expected: rvalue reference to a binding wrapper is rejected.
    app.mapGet("/users/{id}", [](mach::Param<int, "id">&& id) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 4
    // Expected: binding wrapper pointers are rejected.
    app.mapGet("/users/{id}", [](mach::Body<int>* body) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 5
    // Expected: cannot mix explicit binding wrappers and ordinary arguments.
    app.mapGet("/users/{id}", [](mach::Param<int, "id"> id, int count) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 6
    // Expected: cannot mix explicit binding wrappers and std::string arguments.
    app.mapGet("/users/{id}", [](mach::Param<int, "id"> id, std::string name) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 7
    // Expected: at most one Context argument in a Minimal API handler.
    app.mapGet(
        "/users/{id}",
        [](mach::Context& first, mach::Context& second, mach::Param<int, "id"> id) {
            return mach::ok();
        });

#elif MACH_COMPILE_FAILURE_CASE == 8
    // Expected: Context passed by value is rejected.
    app.mapGet("/users/{id}", [](mach::Context context, mach::Param<int, "id"> id) {
        return mach::ok();
    });

#elif MACH_COMPILE_FAILURE_CASE == 9
    // Expected: Context pointer is rejected.
    app.mapGet("/users/{id}", [](mach::Context* context, mach::Param<int, "id"> id) {
        return mach::ok();
    });
#endif
}

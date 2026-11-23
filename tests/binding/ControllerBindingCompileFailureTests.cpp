
// Negative compilation tests for controller action signatures.
// Compile each case separately with -DMACH_CONTROLLER_COMPILE_FAILURE_CASE=N.
// Case 0 is a valid baseline; cases 1-11 must fail to compile.

#include <string>
#include <string_view>

#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/Reply.hpp>
#include <mach/binding/Body.hpp>
#include <mach/binding/Param.hpp>
#include <mach/binding/Query.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>

#ifndef MACH_CONTROLLER_COMPILE_FAILURE_CASE
#error "Define MACH_CONTROLLER_COMPILE_FAILURE_CASE (0 through 11)"
#endif

#if MACH_CONTROLLER_COMPILE_FAILURE_CASE < 0 || MACH_CONTROLLER_COMPILE_FAILURE_CASE > 11
#error "MACH_CONTROLLER_COMPILE_FAILURE_CASE must be between 0 and 11"
#endif

namespace
{

    struct UserDto {
        int id;
    };

    class CompileFailureController final : public mach::ControllerBase {
    public:
        inline static constexpr std::string_view route = "/compile-failure";

        static void configure(mach::ControllerBuilder<CompileFailureController>& builder) {
            builder.mapGet("/{id}", &CompileFailureController::action);
        }

#if MACH_CONTROLLER_COMPILE_FAILURE_CASE == 0
        // Positive compilation baseline.
        mach::Reply<int> action(mach::Param<int, "id"> id) {
            return ok(id.value());
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 1
        // Binding lvalue reference must be rejected.
        mach::Reply<int> action(mach::Param<int, "id">& id) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 2
        // Binding const lvalue reference must be rejected.
        mach::Reply<int> action(const mach::Param<int, "id">& id) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 3
        // Binding rvalue reference must be rejected.
        mach::Reply<int> action(mach::Body<int>&& body) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 4
        // Top-level const binding by value must be rejected.
        mach::Reply<int> action(mach::Query<int, "page">* page) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 5
        // Explicit binding and ordinary integer must not be mixed.
        mach::Reply<int> action(mach::Param<int, "id"> id, int count) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 6
        // Explicit binding and ordinary string must not be mixed.
        mach::Reply<int> action(mach::Query<int, "page"> page, std::string name) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 7
        // Context reference is not an explicit controller binding.
        mach::Reply<int> action(mach::Context& ctx, mach::Param<int, "id"> id) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 8
        // Const Context reference is not an explicit controller binding.
        mach::Reply<int> action(const mach::Context& ctx, mach::Param<int, "id"> id) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 9
        // Legacy controller actions accept at most one body argument.
        mach::Reply<int> action(UserDto first, UserDto second) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 10
        // Context by value must not be mixed with an explicit binding.
        mach::Reply<int> action(mach::Context ctx, mach::Param<int, "id"> id) {
            return ok(0);
        }

#elif MACH_CONTROLLER_COMPILE_FAILURE_CASE == 11
        // Context pointer must not be mixed with an explicit binding.
        mach::Reply<int> action(mach::Context* ctx, mach::Param<int, "id"> id) {
            return ok(0);
        }
#endif
    };

} // namespace

int main() {
    auto app = mach::AppBuilder().addController<CompileFailureController>().build();
}

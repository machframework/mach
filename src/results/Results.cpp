#include <mach/results/Results.hpp>

namespace mach
{
    Reply<std::string> ok(const char* value) {
        return ok(std::string(value));
    }

    Reply<> ok() {
        return Reply(http::StatusCode::Ok);
    }

    Reply<std::string> created(const char* value) {
        return created(std::string(value));
    }

    Reply<> created() {
        return Reply(http::StatusCode::Created);
    }

    Reply<> noContent() {
        return Reply(http::StatusCode::NoContent);
    }
}

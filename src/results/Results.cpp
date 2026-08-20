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

    Reply<std::string> badRequest(const char* value) {
        return badRequest(std::string(value));
    }

    Reply<std::string> unauthorized(const char* value) {
        return unauthorized(std::string(value));
    }

    Reply<std::string> forbidden(const char* value) {
        return forbidden(std::string(value));
    }

    Reply<std::string> notFound(const char* value) {
        return notFound(std::string(value));
    }

    Reply<std::string> conflict(const char* value) {
        return conflict(std::string(value));
    }
}

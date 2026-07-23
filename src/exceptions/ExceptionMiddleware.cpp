#include <mach/detail/exceptions/ExceptionMiddleware.hpp>

#include <stdexcept>

#include <mach/http/StatusCode.hpp>
#include <mach/logging/Logging.hpp>

#include <mach/detail/exceptions/BodyBindingException.hpp>

namespace mach::detail::exceptions
{
    void ExceptionMiddleware::invoke(mach::Context& context, mach::Next& next) {
        try {
            next();
        } catch (const BodyBindingException& ex) {
            // later, initialize a new response inside context
            logging::Logger::error(ex.what());
            context.response.status(mach::http::StatusCode::BadRequest);
            context.response.body(ex.what());
        } catch (const std::exception& ex) {
            // later, initialize a new response inside context
            logging::Logger::error(ex.what());
            context.response.status(mach::http::StatusCode::InternalServerError);
            context.response.body(ex.what());
        } catch (...) {
            // later, initialize a new response inside context
            logging::Logger::error("Request handling failed with unknown exception");
            context.response.status(mach::http::StatusCode::InternalServerError);
            context.response.body(
                std::string(mach::http::reasonPhrase(mach::http::StatusCode::InternalServerError)));
        }
    }
}

#include <mach/detail/exceptions/ExceptionMiddleware.hpp>

#include <stdexcept>

#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>
#include <mach/Logger.hpp>
#include <mach/exceptions/HttpException.hpp>
#include <mach/http/StatusCode.hpp>
#include <mach/Response.hpp>

#include <mach/detail/exceptions/BodyBindingException.hpp>

namespace mach::detail::exceptions
{
    ExceptionMiddleware::ExceptionMiddleware(Logger& logger) : m_logger(logger) {}

    void ExceptionMiddleware::invoke(mach::Context& context, mach::Next& next) {
        try {
            next();
        } catch (const BodyBindingException& ex) {
            m_logger.error("{}", ex.what());

            context.response = Response{};
            context.response.status(mach::http::StatusCode::BadRequest);
            context.response.body(ex.what());
        } catch (const HttpException& ex) {
            m_logger.error("{}", ex.what());

            context.response = Response{};
            context.response.status(ex.status());
            context.response.body(ex.what());
        } catch (const std::exception& ex) {
            m_logger.error("{}", ex.what());

            context.response = Response{};
            context.response.status(mach::http::StatusCode::InternalServerError);
            context.response.body(ex.what());
        } catch (...) {
            m_logger.error("Request handling failed with unknown exception");

            context.response = Response{};
            context.response.status(mach::http::StatusCode::InternalServerError);
            context.response.body(
                std::string(mach::http::reasonPhrase(mach::http::StatusCode::InternalServerError)));
        }
    }
}

#include <mach/controllers/ControllerBase.hpp>

namespace mach
{
    mach::Request& ControllerBase::request() noexcept {
        return m_context->request;
    }

    const mach::Request& ControllerBase::request() const noexcept {
        return m_context->request;
    }

    mach::Response& ControllerBase::response() noexcept {
        return m_context->response;
    }

    const mach::Response& ControllerBase::response() const noexcept {
        return m_context->response;
    }

    mach::Reply<std::string> ControllerBase::ok(const char* value) {
        return mach::ok(value);
    }

    mach::Reply<> ControllerBase::ok() {
        return mach::ok();
    }

    mach::Reply<std::string> ControllerBase::created(const char* value) {
        return mach::created(value);
    }

    mach::Reply<> ControllerBase::created() {
        return mach::created();
    }

    mach::Reply<> ControllerBase::noContent() {
        return mach::noContent();
    }

    mach::Reply<std::string> ControllerBase::badRequest(const char* value) {
        return mach::badRequest(value);
    }

    mach::Reply<std::string> ControllerBase::unauthorized(const char* value) {
        return mach::unauthorized(value);
    }

    mach::Reply<std::string> ControllerBase::forbidden(const char* value) {
        return mach::forbidden(value);
    }

    mach::Reply<std::string> ControllerBase::notFound(const char* value) {
        return mach::notFound(value);
    }

    mach::Reply<std::string> ControllerBase::conflict(const char* value) {
        return mach::conflict(value);
    }

    void ControllerBase::setContext(mach::Context& context) noexcept {
        m_context = &context;
    }
}

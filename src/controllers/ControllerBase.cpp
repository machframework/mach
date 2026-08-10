#include <mach/controllers/ControllerBase.hpp>

namespace mach
{
    mach::Reply<std::string> ControllerBase::ok(const char* value) {
        return mach::ok(value);
    }

    mach::Reply<std::string> ControllerBase::created(const char* value) {
        return mach::created(value);
    }

    mach::Reply<> ControllerBase::noContent() {
        return mach::noContent();
    }

    [[nodiscard]]
    mach::Request& ControllerBase::request() noexcept {
        return m_context->request;
    }

    [[nodiscard]]
    const mach::Request& ControllerBase::request() const noexcept {
        return m_context->request;
    }

    [[nodiscard]]
    mach::Response& ControllerBase::response() noexcept {
        return m_context->response;
    }

    [[nodiscard]]
    const mach::Response& ControllerBase::response() const noexcept {
        return m_context->response;
    }
}

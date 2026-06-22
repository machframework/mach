#pragma once

template <typename TController, typename THandler>
class ControllerActionDescriptor final : public IControllerActionDescriptor {
public:
    explicit ControllerActionDescriptor(THandler handler)
        : m_handler(handler) {
    }

    THandler handler() const {
        return m_handler;
    }

private:
    THandler m_handler;
};
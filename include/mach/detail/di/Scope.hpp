#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace mach::detail::di
{
    class Container;

    class Scope {

    public:
        template <typename T>
        T& resolve();

    private:
        explicit Scope(Container& container);

        std::shared_ptr<void> resolve(std::type_index type);

        std::unordered_map<std::type_index, std::shared_ptr<void>> m_scopedInstances;
        std::vector<std::shared_ptr<void>> m_transientInstances;

        std::vector<std::type_index> m_resolutionStack;

        friend class Container;
        Container& m_container;
    };

    template <typename T>
    T& Scope::resolve() {
        return *std::static_pointer_cast<T>(resolve(std::type_index(typeid(T))));
    }
}

#include "Container.hpp"

namespace mach::detail::di
{
	const ServiceDescriptor& Container::getDescriptor(std::type_index type) const {
		return m_serviceRegistry.find(type)->second;
	}

	Scope Container::createScope() {
		return Scope(*this);
	}

	std::shared_ptr<void> Container::getOrCreateSingleton(std::type_index type, Scope& scope) {
		if (m_singletonInstances.contains(type)) {
			return m_singletonInstances.find(type)->second;
		}

		auto instance = m_serviceRegistry.find(type)->second.factory(scope);
		m_singletonInstances.emplace(type, instance);

		return instance;
	}
}

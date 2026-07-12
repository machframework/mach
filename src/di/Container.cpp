#include <mach/detail/di/Container.hpp>

#include <format>
#include <stdexcept>

namespace mach::detail::di
{
	const ServiceDescriptor* Container::getDescriptor(std::type_index type) const {
		const auto descriptor = m_serviceRegistry.find(type);

		if(descriptor == m_serviceRegistry.end()) {
			return nullptr;
		}

		return &descriptor->second;
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

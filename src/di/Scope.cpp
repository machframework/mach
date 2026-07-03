#include <mach/detail/di/Scope.hpp>

#include <format>
#include <stdexcept>

#include <mach/detail/di/Container.hpp>

namespace mach::detail::di
{
	std::shared_ptr<void> Scope::resolve(std::type_index type) {
		if (m_scopedInstances.contains(type)) {
			return m_scopedInstances.find(type)->second;
		}

		const auto& descriptor = m_container.getDescriptor(type);

		if (descriptor.lifetime == ServiceLifetime::Singleton) {
			return m_container.getOrCreateSingleton(descriptor.type, *this);
		}

		auto instance = descriptor.factory(*this);

		if (descriptor.lifetime == ServiceLifetime::Scoped) {
			m_scopedInstances.emplace(descriptor.type, instance);
		}
		else {
			m_transientInstances.push_back(instance);
		}

		return instance;
	}

	Scope::Scope(Container& container) 
		: m_container(container)
	{ 
		s_createdScopes++;
		s_aliveScopes++;
	}
}

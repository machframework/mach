#include <mach/detail/di/Container.hpp>

#include <format>
#include <mutex>
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

	void Container::finalizeRegistrations() {
		for (const auto& [type, descriptor] : m_serviceRegistry) {
			if (descriptor.lifetime == ServiceLifetime::Singleton) {
				m_singletonEntries.try_emplace(
					type,
					std::make_unique<SingletonEntry>()
				);
			}
		}
	}

	std::shared_ptr<void> Container::getOrCreateSingleton(std::type_index type, Scope& scope) {
		// TODO: make thread-safe for first call of each singleton
		auto& entry = *m_singletonEntries.at(type);
		const auto& descriptor = m_serviceRegistry.at(type);

		std::call_once(
			entry.initializationFlag,
			[&] {
				entry.instance = descriptor.factory(scope);
			}
		);

		return entry.instance;
	}
}

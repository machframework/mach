#include <mach/detail/di/Container.hpp>

#include <vector>

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
				const auto scopedDependency = findScopedDependency(descriptor);

				if (scopedDependency) {
					throw std::logic_error(
						"Mach error: singleton service '" +
						std::string(type.name()) +
						"' cannot depend directly or indirectly on scoped service '" +
						std::string(scopedDependency->name()) +
						"'"
					);
				}

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

	std::optional<std::type_index> Container::findScopedDependency(const ServiceDescriptor& descriptor) const {
		for (const auto dependencyType : descriptor.dependencies) {
			const auto& dependency =
				m_serviceRegistry.at(dependencyType);

			if (dependency.lifetime == ServiceLifetime::Scoped) {
				return dependencyType;
			}

			if (auto scopedDependency =
				findScopedDependency(dependency)) {
				return scopedDependency;
			}
		}

		return std::nullopt;
	}
}

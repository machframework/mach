#pragma once

#include <atomic>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <mach/detail/di/ServiceDescriptor.hpp>

namespace mach::detail::di
{
	class Container;

	class Scope {

	public:
		~Scope() {
			s_aliveScopes--;
		}

		static std::int64_t aliveCount() {
			return s_aliveScopes.load();
		}

		static std::int64_t createdCount() {
			return s_createdScopes.load();
		}

		template <typename T>
		T& resolve();

	private:
		explicit Scope(Container& container);

		std::shared_ptr<void> resolve(std::type_index type);

		std::unordered_map<std::type_index, std::shared_ptr<void>> m_scopedInstances;
		std::vector<std::shared_ptr<void>> m_transientInstances;

		Container& m_container;

		static inline std::atomic<std::int64_t> s_createdScopes = 0;
		static inline std::atomic<std::int64_t> s_aliveScopes = 0;

		friend class Container;
	};

	template <typename T>
	T& Scope::resolve() {		
		return *std::static_pointer_cast<T>(
			resolve(std::type_index(typeid(T)))
		);
	}
}

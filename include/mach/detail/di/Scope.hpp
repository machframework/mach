#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include <mach/detail/di/ServiceDescriptor.hpp>

namespace mach::detail::di
{
	class Container;

	class Scope {

	public:
		template <typename T>
		std::shared_ptr<T> resolve();

		std::shared_ptr<void> resolve(std::type_index type);

	private:
		explicit Scope(Container& container);

		std::unordered_map<std::type_index, std::shared_ptr<void>> m_scopedInstances;
		Container& m_container;

		friend class Container;
	};

	template <typename T>
	std::shared_ptr<T> Scope::resolve() {		
		return std::static_pointer_cast<T>(resolve(std::type_index(typeid(T))));
	}
}

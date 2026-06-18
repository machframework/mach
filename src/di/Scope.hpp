#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ServiceDescriptor.hpp"

namespace mach::detail::di
{
	class Container;

	class Scope {

	public:
		template <typename T>
		std::shared_ptr<T> resolve() const;

		std::shared_ptr<void> resolve(std::type_index type) const;

	private:
		Scope(const Container& container);

		std::unordered_map<std::type_index, std::shared_ptr<void>> m_scopedInstances;
		const Container& m_container;

		friend class Container;
	};

	template <typename T>
	std::shared_ptr<T> Scope::resolve() const {		
		return std::static_pointer_cast<T>(resolve(std::type_index(typeid(T))));
	}
}

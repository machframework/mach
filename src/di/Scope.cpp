#include "Scope.hpp"

#include "Container.hpp"

namespace mach::detail::di
{
	std::shared_ptr<void> Scope::resolve(std::type_index type) const {
		if (m_scopedInstances.contains(type)) {
			return m_scopedInstances.find(type)->second;
		}

		const auto& descriptor = m_container.getDescriptor(type);
		return descriptor.factory(*this);
	}

	Scope::Scope(const Container& container) 
		: m_container(container)
	{ }
}

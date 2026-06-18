#include "Container.hpp"

namespace mach::detail::di
{
	const ServiceDescriptor& Container::getDescriptor(std::type_index type) const {
		return m_serviceRegistry.find(type)->second;
	}

	Scope Container::createScope() const {
		return Scope(*this);
	}
}

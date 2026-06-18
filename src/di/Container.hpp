#pragma once

#include <memory>
#include <typeindex>

#include "Scope.hpp"
#include "ServiceDescriptor.hpp"

namespace mach::detail::di
{
	class Container {
		
	public:
		void addService(ServiceDescriptor descriptor);

		std::shared_ptr<void> resolve(std::type_index type) const;
		Scope createScope() const;

	private:
		std::unordered_map<std::type_index, ServiceDescriptor> m_serviceRegistry;
	};
}

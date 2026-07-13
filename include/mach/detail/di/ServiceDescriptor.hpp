#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <vector>

#include <mach/detail/di/ServiceLifetime.hpp>

namespace mach::detail::di
{
	class Scope;

	struct ServiceDescriptor {
		std::type_index type;
		ServiceLifetime lifetime;
		std::vector<std::type_index> dependencies;
		std::function<std::shared_ptr<void>(Scope&)> factory;
	};
}

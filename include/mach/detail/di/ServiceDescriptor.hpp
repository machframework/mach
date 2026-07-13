#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <vector>

#include <mach/detail/di/ServiceLifetime.hpp>

namespace mach::detail::di
{
	class Scope;

	enum class ServiceAccess {
		User,
		Internal
	};

	struct ServiceDescriptor {
		std::type_index type;
		ServiceLifetime lifetime;
		ServiceAccess access;
		std::vector<std::type_index> dependencies;
		std::function<std::shared_ptr<void>(Scope&)> factory;
	};
}

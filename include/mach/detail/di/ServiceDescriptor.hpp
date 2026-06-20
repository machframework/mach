#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include <mach/detail/di/ServiceLifetime.hpp>

namespace mach::detail::di
{
	class Scope;

	struct ServiceDescriptor {
		std::type_index type;
		ServiceLifetime lifetime;
		std::function<std::shared_ptr<void>(Scope&)> factory;
	};
}

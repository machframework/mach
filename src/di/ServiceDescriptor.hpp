#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "ServiceLifetime.hpp"
#include "Scope.hpp"

namespace mach::detail::di
{
	struct ServiceDescriptor {
		std::type_index type;
		ServiceLifetime lifetime;
		std::function<std::shared_ptr<void>(Scope&)> factory;
	};
}

#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "ServiceLifetime.hpp"

namespace mach::detail::di
{
	class Scope;

	struct ServiceDescriptor {
		std::type_index type;
		ServiceLifetime lifetime;
		std::function<std::shared_ptr<void>(const Scope&)> factory;
	};
}

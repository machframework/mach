#pragma once

namespace mach
{
	enum class ServiceLifetime {
		Scoped,
		Singleton,
		Transient
	};
}

#pragma once

namespace mach::detail::di
{
	enum class ServiceLifetime {
		Scoped,
		Singleton,
		Transient
	};
}

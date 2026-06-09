#pragma once

#include <mach/Context.hpp>

#include "routing/Router.hpp"

namespace mach::detail::application
{
	class Runtime {

	public:
		void handle(mach::Context& context);

	private:
		routing::Router m_router;
	};
}

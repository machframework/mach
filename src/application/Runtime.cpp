#include "Runtime.hpp"

namespace mach::detail::application {
	void Runtime::handle(mach::Context& context) {
		context.response.body(context.request.body());
	}
}

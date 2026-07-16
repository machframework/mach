#include <string_view>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/results/Reply.hpp>

#include "Testing.hpp"

class GeneralController : public mach::ControllerBase {

public:
	inline static constexpr std::string_view route = "/general";

	mach::Reply<int> getAge() {
		return ok(5);
	}

	int getAge2() {
		return 5;
	}

	static void configure(mach::ControllerBuilder<GeneralController>& routes) {
		routes.mapGet("/age", & GeneralController::getAge);
		routes.mapGet("/age2", & GeneralController::getAge2);
	}
};

int main() {
	auto builder = mach::AppBuilder(std::move(testing::serverOptions));
	builder.addController<GeneralController>();

	auto app = builder.build();
	app.mapController<GeneralController>();

	return app.run();
}

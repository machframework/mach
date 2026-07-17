#include <string_view>
#include <mutex>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/controllers/ControllerBuilder.hpp>
#include <mach/diagnostics/TerminateHandler.hpp>
#include <mach/Json.hpp>
#include <mach/results/Reply.hpp>

#include "Testing.hpp"

struct Person {
	std::string name;
	int age;
	bool male;
};

MACH_DEFINE_JSON(Person, name, age, male);

class GeneralController : public mach::ControllerBase {

public:
	inline static constexpr std::string_view route = "/general";

	mach::Reply<int> getAge(std::string name) {
		return ok(5);
	}

	static void configure(mach::ControllerBuilder<GeneralController>& routes) {
		routes.mapGet("/age", &GeneralController::getAge);
	}
};

int main() {
	mach::installTerminateHandler();

	auto builder = mach::AppBuilder(std::move(testing::serverOptions));
	builder.addController<GeneralController>();

	auto app = builder.build();
	app.mapController<GeneralController>();

	return app.run();
}

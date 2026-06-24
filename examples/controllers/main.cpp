#include <iostream>
#include <string>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/results/Reply.hpp>

class HomeController : public mach::ControllerBase {

public:
	inline static std::string route = "/home";

	[[mach::get("/users/age/{birth:int}")]]
	mach::Reply<int> calculateAge() {
		const int currentYear = 2026;
		int birth = std::stoi(std::string(context->request.routeParam("birth")));
		int age = currentYear - birth;

		std::cout << "My age is: " << age << std::endl;

		return ok(age);
	}

	[[mach::get("")]]
	mach::Reply<std::string> sayHi() {
		return ok("Hi");
	}

	static void configure(mach::ControllerBuilder<HomeController>& routes) {
		routes.get("/age/{birth:int}", &HomeController::calculateAge);
		routes.get(& HomeController::sayHi);
	}
};

int main() {
	auto builder = mach::AppBuilder("127.0.0.1", 3143, 12);
	builder.addController<HomeController>();

	auto app = builder.build();
	app.mapController<HomeController>();

	app.run();

	return 0;
}

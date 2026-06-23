#include <iostream>
#include <string>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/results/Reply.hpp>

class HomeController : public mach::ControllerBase {

public:
	inline static std::string route = "/home";

	[[mach::get("/users")]]
	mach::Reply<int> calculateAge() {
		const int currentYear = 2026;
		int birth = std::stoi(std::string(context->request.routeParam("birth")));
		int age = currentYear - birth;

		std::cout << "My age is: " << age << std::endl;

		return ok(age);
	}

	static void configure(mach::ControllerBuilder<HomeController>& methods) {
		methods.get("/age/{birth:int}", &HomeController::calculateAge);
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

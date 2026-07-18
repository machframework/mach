#include <iostream>
#include <string>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/Json.hpp>
#include <mach/results/Reply.hpp>
#include <mach/StringConversion.hpp>

struct Person {
	std::string name;
	int age;
};

MACH_DEFINE_JSON(Person, name, age)

class HomeController : public mach::ControllerBase {

public:
	inline static std::string route = "/home";

	[[mach::get]]
	mach::Reply<int> calculateAge() {
		constexpr int currentYear = 2026;
		
		int birthYear = context->request.routeParam<int>("birth");
		int age = currentYear - birthYear;

		std::cout << "I am " << age << "\n";

		return ok(age);
	}

	[[mach::get("/hi")]]
	mach::Reply<std::string> sayHi() {
		return ok("Hi");
	}
	
	static void configure(mach::ControllerBuilder<HomeController>& routes) {
		routes.mapGet("/{birth:int}", &HomeController::calculateAge);
		routes.mapGet("/hi" ,& HomeController::sayHi);
	}
};

int main() {
	auto builder = mach::AppBuilder("127.0.0.1", 3143, 12);
	builder.addController<HomeController>();

	auto app = builder.build();
	return app.run();
}

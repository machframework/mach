#include <iostream>
#include <string>

#include <mach/AppBuilder.hpp>
#include <mach/controllers/ControllerBase.hpp>
#include <mach/results/Reply.hpp>

#include <mach/Json.hpp>

struct Person {
	std::string name;
	int age;
};

MACH_DEFINE_JSON(Person, name, age)

class HomeController : public mach::ControllerBase {

public:
	inline static std::string route = "/home";

	[[mach::get]]
	mach::Reply<Person> calculateAge(Person person) {
		const int currentYear = 2026;
		
		std::cout << "My name is " << person.name
			<< " and I am " << person.age << "\n";

		return ok(person);
	}

	[[mach::get("/hi")]]
	mach::Reply<std::string> sayHi() {
		return ok("Hi");
	}

	static void configure(mach::ControllerBuilder<HomeController>& routes) {
		routes.get(&HomeController::calculateAge);
		routes.get("/hi" ,& HomeController::sayHi);
	}
};

int main() {
	auto builder = mach::AppBuilder("127.0.0.1", 3143, 16);
	builder.addController<HomeController>();

	auto app = builder.build();
	app.mapController<HomeController>();

	app.run();

	return 0;
}

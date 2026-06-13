#include <iostream>
#include <thread>

#include <mach/App.hpp>
#include <mach/Context.hpp>

int main() {

	try {
		int threads = std::thread::hardware_concurrency();
		auto app = mach::App("127.0.0.1", 3143, threads);

		app.get("/users/{name}", [](mach::Context& context) {
			std::cout << "My name is " << context.request.routeParam("name") << std::endl;
		});

		app.get("/users/{name}/{age:int}", [](mach::Context& context) {
			std::cout << "My name is: " << context.request.routeParam("name")
				<< " and my age is: " << context.request.routeParam("age") << std::endl;
		});

		app.run();
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}

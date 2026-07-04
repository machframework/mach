#include <iostream>
#include <thread>

#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>

#include <mach/results/Reply.hpp>

int main() {

	try {
		int threads = std::thread::hardware_concurrency();

		auto builder = mach::AppBuilder("127.0.0.1", 3143, threads);
		auto app = builder.build();

		app.mapGet("/users/{name:string}", [](mach::Context& context) {
			std::cout << "My name is " << context.request.routeParam("name") << std::endl;
		});

		app.mapGet("/users/{name}/{age:int}", [](mach::Context& context) {
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

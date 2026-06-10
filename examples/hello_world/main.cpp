#include <iostream>
#include <thread>

#include <mach/App.hpp>
#include <mach/Context.hpp>

int main() {

	try {
		int threads = std::thread::hardware_concurrency();
		auto app = mach::App("127.0.0.1", 3143, threads);

		app.addRoute(mach::http::Method::Get, "/users", [](mach::Context& context) {
			std::cout << "Request Body: " << context.request.body() << std::endl;
			});

		app.addRoute(mach::http::Method::Get, "/users/{age}/{age}", [](mach::Context& context) {
			std::cout << "Target: " << context.request.target() << std::endl;
			});

		app.run();
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}

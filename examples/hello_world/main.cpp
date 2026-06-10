#include <iostream>
#include <thread>

#include <mach/App.hpp>
#include <mach/Context.hpp>

int main() {
	int threads = std::thread::hardware_concurrency();

	auto app = mach::App("127.0.0.1", 3143, threads);

	app.addRoute(mach::http::Method::Get, "/users", [](mach::Context& context) {
		std::cout << "Request Body: " << context.request.body() << std::endl;
	});

	app.run();
}

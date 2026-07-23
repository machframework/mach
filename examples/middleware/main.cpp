#include <iostream>
#include <string>
#include <thread>

#include <mach/mach.hpp>
#include <mach/middleware/Next.hpp>

class AuthMiddleware {

public:
	void invoke(mach::Context& context, mach::Next& next) {
		std::cout << "Before:\n";
		next();
		std::cout << "After:\n";
	}
};

int main() {
	int threads = std::thread::hardware_concurrency();

	auto builder = mach::AppBuilder("127.0.0.1", 3143, threads);
	builder.use<AuthMiddleware>();

	auto app = builder.build();

	app.mapGet("/test", [](mach::Context& context) {
		std::cout << "Inside handler\n";
		context.response.body("Reached");
	});

	return app.run();
}

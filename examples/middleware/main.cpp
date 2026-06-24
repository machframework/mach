#include <iostream>
#include <string>
#include <thread>

#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>
#include <mach/middleware/Next.hpp>

#include <mach/detail/dispatching/RequestExecution.hpp>

//
#include <mach/detail/middleware/MiddlewarePipeline.hpp>
//

class AuthMiddleware {

public:
	void invoke(mach::Context& context, mach::Next next) {
		std::cout << "Before:\n";
		next(context);
		std::cout << "After:\n";
	}
};

int main() {
	int threads = std::thread::hardware_concurrency();

	auto builder = mach::AppBuilder("127.0.0.1", 3143, threads);
	builder.use<AuthMiddleware>();

	auto app = builder.build();
	app.get("/test", [](mach::Context& context) {
		context.response.body("Reached");
	});
}

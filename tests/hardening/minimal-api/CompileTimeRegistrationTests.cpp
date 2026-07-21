#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>

#include "Testing.hpp"

static void doNothing() {

}

int main() {
	mach::AppBuilder builder(std::move(testing::serverOptions));
	auto app = builder.build();

	app.mapGet("/noexcept", []() noexcept {

		});
	
	app.mapGet("/free-function", doNothing);

	app.mapGet("/pointer", &doNothing);

	return app.run();
}
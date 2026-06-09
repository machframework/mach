#include <thread>

#include <mach/App.hpp>

int main() {
	int threads = std::thread::hardware_concurrency();

	auto app = mach::App("127.0.0.1", 3143, threads);
	app.run();

	return 0;
}

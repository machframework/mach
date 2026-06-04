#include <thread>

#include <mach/server/Server.hpp>

int main() {
	int threads = std::thread::hardware_concurrency();

	auto server = mach::Server("0.0.0.0", 3143, threads);
	server.run();

	return 0;
}
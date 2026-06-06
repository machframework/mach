#include <thread>

#include <mach/server/Server.hpp>

int main() {
	int threads = std::thread::hardware_concurrency();

	auto server = mach::Server("127.0.0.1", 3143, threads);
	server.run();

	return 0;
}

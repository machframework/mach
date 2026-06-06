#pragma once

#include <memory>
#include <string>

namespace mach::detail::server
{
	class Server {

	public:
		Server(const std::string& host, std::uint16_t port, std::size_t thread_count = 1);
		~Server();
		
		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void run();

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}

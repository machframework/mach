#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "application/Runtime.hpp"

namespace mach::detail::server
{
	class Server {

	public:
		Server(const std::string_view& host, std::uint16_t port, std::size_t thread_count, application::Runtime& runtime);
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

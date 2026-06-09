#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace mach
{
	class App {

	public:
		App(std::string_view host, std::uint16_t port, std::size_t threadCount = 1);
		~App();

		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void run();

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}

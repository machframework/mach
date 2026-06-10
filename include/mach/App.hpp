#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <mach/http/Method.hpp>
#include <mach/Context.hpp>

// TODO: change registration method to a template
#include "../src/core/Handler.hpp"

namespace mach
{
	class App {

	public:
		App(std::string_view host, std::uint16_t port, std::size_t threadCount = 1);
		~App();

		std::string host() const noexcept;
		std::uint16_t port() const noexcept;
		std::size_t threadCount() const noexcept;

		void addRoute(mach::http::Method method, std::string pattern, detail::Handler handler);

		void run();

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}

#include <mach/AppBuilder.hpp>

#include <mach/detail/binding/BodyBinder.hpp>

#include <iostream>

namespace mach
{
	AppBuilder::AppBuilder(std::string_view host, std::uint16_t port, std::size_t threadCount) {
		m_serverOptions = detail::app::ServerOptions{
			std::string(host),
			port, threadCount
		};

		// register preprocessing middleware
		// register routing middleware
	}

	App AppBuilder::build(){
		m_container.addService<detail::binding::BodyBinder>(detail::di::ServiceLifetime::Singleton);

		return mach::App(
			std::move(m_serverOptions),
			std::move(m_container),
			std::move(m_middlewarePipeline)
		);
	}
}

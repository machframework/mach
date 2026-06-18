#pragma once

#include <string_view>

#include <mach/App.hpp>

#include "app/ServerOptions.hpp"
#include "di/Container.hpp"
#include "di/ServiceLifetime.hpp"


namespace mach
{
	class AppBuilder {

	public:
		AppBuilder(std::string_view host, std::uint16_t port, std::size_t threadCount = 1);

		template <typename T, typename... Deps>
		AppBuilder& addScoped();

		template <typename T, typename... Deps>
		AppBuilder& addSingleton();

		template <typename T, typename... Deps>
		AppBuilder& addTransient();

	private:
		detail::app::ServerOptions m_serverOptions;
		detail::di::Container m_container;
	};

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addScoped() {
		m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Scoped);
		return *this;
	}

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addSingleton() {
		m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Singleton);
		return *this;
	}

	template <typename T, typename... Deps>
	AppBuilder& AppBuilder::addTransient() {
		m_container.addService<T, Deps...>(detail::di::ServiceLifetime::Transient);
		return *this;
	}
}

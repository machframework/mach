# Changelog

All notable changes to Mach are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed

- Respect the configured MSVC runtime during builds.
- Separate package generation from installation.

## [1.0.0] - 2026-09-24

### Added

- Initial public release of Mach, a modern C++20 web framework.
- HTTP server built on Boost.Asio and Boost.Beast.
- Routing with typed parameters, constraints, and HTTP method handling.
- Controller-based and minimal API endpoint definitions.
- Dependency injection with singleton, scoped, and transient lifetimes.
- Middleware pipeline and exception handling.
- JSON serialization, deserialization, and request body binding.
- Request and response utilities, including cookies, CORS, and CSRF protection.
- Mach CLI for project creation, code generation, building, and running applications.
- Windows and Ubuntu installation packages.
- Documentation, examples, and performance benchmarks.

[Unreleased]: https://github.com/machframework/mach/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/machframework/mach/releases/tag/v1.0.0

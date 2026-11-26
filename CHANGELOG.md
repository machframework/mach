# Changelog

All notable changes to Mach are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Add SI and IEC byte-size literals.
- Add configurable HTTP request body, header, and target size limits.
- Add fluent application configuration methods to `AppBuilder`.
- Automatically install the termination handler when building an application, with an option to disable it.
- Add explicit request binding sources through `Body<T>`, `Param<T, Name>`, and `Query<T, Name>`.
- Support combining multiple request binding sources in minimal API handlers and controller actions.
- Add 23 convenience getters for common HTTP request headers.
- Add 27 convenience getters for common HTTP response headers.
- Add 26 convenience setters for common HTTP response headers.
- Add 7 typed response header setter overloads for HTTP dates, durations, and method lists.
- Add media type and charset inspection methods to `Request` and `Response`, including JSON and form media type detection.
- Add numeric Content-Length inspection to `Request` and `Response`.
- Add authentication scheme and Bearer token inspection to `Request`.
- Add `setAttachment()` and `setInline()` to `Response`, with UTF-8 filename support, ASCII fallback generation, and filename validation.
- Add automated cross-platform regression testing through GitHub Actions.

### Changed

- Strengthen HTTP media type and charset parsing, including validation of malformed and duplicate charset parameters.
- Simplify the CMake testing infrastructure and eliminate redundant test registrations while preserving existing test coverage.
- Change `AppOptions::port` and the parameter type of `AppBuilder::port()` from `std::uint16_t` to `std::uint32_t`.

### Deprecated

- Deprecate manual termination handler installation.

### Fixed

- Respect the configured MSVC runtime during builds.
- Separate package generation from installation.
- Remove incorrect `noexcept` specifications from HTTP lookup methods that may allocate memory. Code explicitly relying on their non-throwing exception specifications may require adjustments.
- Prevent silent truncation of out-of-range port values supplied through `AppBuilder::port()` or `AppOptions::port`, ensuring that invalid values within the `std::uint32_t` range are rejected during application construction.

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

[Unreleased]: https://github.com/machframework/mach/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/machframework/mach/releases/tag/v1.0.0
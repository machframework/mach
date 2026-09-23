# Mach

[![Release](https://img.shields.io/github/v/release/machframework/mach)](https://github.com/OWNER/mach/releases/latest)
[![License](https://img.shields.io/github/license/machframework/mach)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)

A modern C++ web framework.

Mach is a fast, lightweight web framework for building HTTP APIs in modern C++. It provides routing, controllers, request binding, validation, middleware, security utilities, configuration, logging, and more through a clean, type-safe API.

## Installation

### Windows

Download and run the latest Mach `.msi` installer.

Mach v1.0 supports **Windows with MSVC**.

To build Mach applications, you will need:

- CMake
- MSVC C++ Build Tools
- Windows SDK

The Mach CLI also requires the Microsoft Visual C++ Redistributable.

### Ubuntu

Install the latest Mach `.deb` package:

```bash
sudo apt install ./mach-1.0.0-amd64.deb
```

Mach v1.0 supports **Ubuntu with GCC**.

To build Mach applications, install the required development tools:

```bash
sudo apt update
sudo apt install build-essential cmake
```

See the [Installation Guide](https://machframework.dev/) for detailed installation instructions.

## Quick Start

Create a new Mach project:

```bash
mach new hello-mach
cd hello-mach
```

Build and run it:

```bash
mach run
```

A minimal Mach application looks like this:

```cpp
#include <mach/mach.hpp>

int main() {
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapGet("/", [] {
        return mach::ok("Hello from Mach!");
    });

    return app.run();
}
```

## Features

- Fast HTTP server built for modern C++
- Minimal APIs and controllers
- Route parameters and query parameters
- JSON request binding and serialization
- Declarative request validation
- Middleware
- CORS and CSRF protection
- Centralized error handling
- Application configuration
- Built-in logging
- CMake integration
- Command-line tooling for creating, building, and running projects

## Platforms

| Platform | Toolchain | Support   |
| -------- | --------- | --------- |
| Windows  | MSVC      | Supported |
| Ubuntu   | GCC       | Supported |

Additional platforms and toolchains may be supported in future releases.

## Documentation

Full documentation, guides, tutorials, and the API reference are available at:

**https://machframework.dev**

## Benchmarks

Mach is designed to provide high HTTP throughput while keeping its API simple and expressive.

See the [benchmarks](https://machframework.dev/) for results and methodology.

## License

Mach is licensed under the [MIT License](LICENSE).

#include <iostream>

#include <mach/AppBuilder.hpp>

int main() {
    mach::App app = mach::AppBuilder("127.0.0.1", 3143, 4).build();
    return app.run();
}

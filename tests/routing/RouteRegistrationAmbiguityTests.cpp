#include <iostream>

#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>

#include "Testing.hpp"

int main() {
    auto app = mach::AppBuilder(std::move(testing::serverOptions)).build();

    // -------------------------
    // Missing leading '/'
    // -------------------------
    {
        const std::string testName = "Path Parameter Ambiguity";

        std::cout << "TEST: " << testName << "\n";

        app.mapGet("/users/{id}/asaf", [](mach::Context& context) {
        });

        try {
            app.mapGet("/users/{id}", [](mach::Context& context) {
            });
        } catch (...) {
            testing::fail(testName, "An unknown exception was thrown");
            return 1;
        }

        try {
            app.mapGet("/users/{id}", [](mach::Context& context) {
            });
        } catch (const std::invalid_argument& ex) {
            std::cout << testing::GREEN
                      << "[SUCCESS] invalid_argument exception thrown! Path parameter duplication "
                         "tests passed!"
                      << " Got: " << ex.what() << testing::RESET << std::endl;
        } catch (...) {
            testing::fail(testName, "An unknown exception was thrown");
            return 1;
        }

        try {
            app.mapGet("/users/{name}", [](mach::Context& context) {
            });
        } catch (const std::invalid_argument& ex) {
            std::cout << testing::GREEN
                      << "[SUCCESS] invalid_argument exception thrown! Path parameter ambiguity "
                         "tests passed!"
                      << " Got: " << ex.what() << testing::RESET << std::endl;
        } catch (...) {
            testing::fail(testName, "An unknown exception was thrown");
            return 1;
        }

        try {
            app.mapGet("/users/{age:int}", [](mach::Context& context) {
            });

            std::cout << testing::GREEN
                      << "[SUCCESS] registration succeeded! Path parameter ambiguity tests passed!"
                      << testing::RESET << std::endl;
        } catch (...) {
            testing::fail(testName, "An unknown exception was thrown");
            return 1;
        }
    }
}

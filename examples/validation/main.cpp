#include <string>

#include <mach/mach.hpp>

struct RegisterRequest {
    std::string username;
    std::string email;
    int age;

    void validate(mach::ValidationBuilder<RegisterRequest>& validation) const {
        validation.field(&RegisterRequest::username).minLength(3).maxLength(30);

        validation.field(&RegisterRequest::email).email();

        validation.field(&RegisterRequest::age).min(18);
    }
};

MACH_DEFINE_JSON(RegisterRequest, username, email, age)

int main() {
    mach::AppBuilder builder;
    auto app = builder.build();

    app.mapPost("/register", [](RegisterRequest request) {
        return mach::ok("Registration accepted");
    });

    return app.run();
}

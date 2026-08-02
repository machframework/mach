#include <mach/ValidationBuilder.hpp>
#include <mach/Json.hpp>

#include "Testing.hpp"

struct TestRequest {
    std::string name;
    int age;

	void validate(mach::ValidationBuilder<TestRequest>& builder) const {
        builder.field(&TestRequest::name).length(1, 50).email();
        builder.field(&TestRequest::age).range(1, 5);
    }
};

MACH_DEFINE_JSON(TestRequest, name, age)

int main() {
	
	return 0;
}

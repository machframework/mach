#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include <mach/Json.hpp>

#include <mach/detail/binding/BodyBinder.hpp>

struct MoveOnlyDto {
    std::string name;
    int age;

    MoveOnlyDto() = default;

    MoveOnlyDto(const MoveOnlyDto&) = delete;
    MoveOnlyDto& operator=(const MoveOnlyDto&) = delete;

    MoveOnlyDto(MoveOnlyDto&&) = default;
    MoveOnlyDto& operator=(MoveOnlyDto&&) = default;
};

MACH_DEFINE_JSON(MoveOnlyDto, name, age);

int main() {
    auto binder = mach::detail::binding::BodyBinder();

    const auto body = R"(
    {
        "name": "Asaf",
        "age": 18
    }
    )";

    auto dto = binder.bind<MoveOnlyDto>(body);

    assert(dto.name == "Asaf");
    assert(dto.age == 18);
}

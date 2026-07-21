#include <cassert>

#include <mach/detail/binding/BodyBinder.hpp>

struct AddressDto
{
    std::string city;
    std::string street;
};

MACH_DEFINE_JSON(AddressDto, city, street);

struct UserDto
{
    std::string name;
    AddressDto address;
};

MACH_DEFINE_JSON(UserDto, name, address);

int main() {
    auto binder = mach::detail::binding::BodyBinder();

    const auto body = R"({
        "name": "asaf",
        "email": "asaf@example.com",
        "address": {
            "country": "Israel",
            "city": "Gedera",
            "street": "Herzl",
            "houseNumber": 12
        }
    })";

    UserDto dto = binder.bind<UserDto>(body);

    assert(dto.name == "asaf");
    assert(dto.address.city == "Gedera");
    assert(dto.address.street == "Herzl");
}
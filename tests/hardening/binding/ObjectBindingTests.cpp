#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <mach/detail/binding/BodyBinder.hpp>

struct AddressDto {
    std::string city;
    std::string street;

    auto operator<=>(const AddressDto&) const = default;
};

MACH_DEFINE_JSON(AddressDto, city, street);

struct UserDto {
    std::string name;
    AddressDto address;
};

MACH_DEFINE_JSON(UserDto, name, address);

int main() {
    auto binder = mach::detail::binding::BodyBinder();

    // nested object binding
    {
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

        const auto dto = binder.bind<UserDto>(body);

        assert(dto.name == "asaf");
        assert(dto.address.city == "Gedera");
        assert(dto.address.street == "Herzl");
    }

    // collection binding
    {
        const auto body = R"(
            [
                {
                    "country": "Israel",
                    "city": "Gedera",
                    "street": "Herzl",
                    "houseNumber": 12
                },
                {
                    "country": "Israel",
                    "city": "Tel Aviv",
                    "street": "Rothschild Boulevard",
                    "houseNumber": 45
                },
                {
                    "country": "Israel",
                    "city": "Jerusalem",
                    "street": "Jaffa Street",
                    "houseNumber": 88
                },
                {
                    "country": "Israel",
                    "city": "Haifa",
                    "street": "HaNassi Boulevard",
                    "houseNumber": 21
                }
            ]
        )";

        const auto addressesVec = binder.bind<std::vector<AddressDto>>(body);

        assert(addressesVec.size() == 4);
        assert(addressesVec[0].city == "Gedera");

        const auto addressesSet = binder.bind<std::set<AddressDto>>(body);

        assert(addressesSet.size() == 4);
    }

    // map binding
    {
        const auto body = R"(
            {
                "home": {
                    "country": "Israel",
                    "city": "Gedera",
                    "street": "Herzl",
                    "houseNumber": 12
                },
                "work": {
                    "country": "Israel",
                    "city": "Tel Aviv",
                    "street": "Rothschild Boulevard",
                    "houseNumber": 45
                },
                "parents": {
                    "country": "Israel",
                    "city": "Haifa",
                    "street": "HaNassi Boulevard",
                    "houseNumber": 21
                }
            }
        )";

        const auto addressesMap = binder.bind<std::map<std::string, AddressDto>>(body);

        assert(addressesMap.size() == 3);
    }
}

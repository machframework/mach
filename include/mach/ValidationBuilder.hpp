#pragma once

#include <mach/detail/validation/FieldValidationBuilder.hpp>

namespace mach
{
    using detail::validation::FieldValidationBuilder;

	template <typename T>
	class ValidationBuilder {

	public:
        template <typename Field>
        FieldValidationBuilder& field(T::Field* field) {
            return FieldValidationBuilder(*this, field);
        }

    private:

	};
}

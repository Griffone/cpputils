#pragma once

#include <type_traits>
#include <variant>

namespace cpputils {
	// A container for runtime-polymorphic objects of common underlying type (Base)
	// All alternatives have to be supplied at template initialization time.
	// Allows easy access to the underlying base class.
	template<class Base, class... Derivatives>
	class BoundedPolymorphism : public std::variant<Derivatives...> {
		template<class Base, class... Derivatives>
		static constexpr bool is_valid_v = (... && std::is_base_of_v<Base, Derivatives>);
		static_assert(is_valid_v<Base, Derivatives...>, "Derivatives have to inherit from Base class!");

	public:
		using variant = std::variant<Derivatives...>;

		// Forward typical std::variant constructors
		using variant::variant;

		// The one new 'feature' over std::variant, dereference any contained class to the Base
		Base* operator->() { return std::visit([](auto& var) { return dynamic_cast<Base*>(&var); }, static_cast<variant&>(*this)); }
		const Base* operator->() const { return std::visit([](const auto& var) { return dynamic_cast<const Base*>(&var); }, static_cast<const variant&>(*this)); }
	};
}
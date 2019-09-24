#pragma once

namespace cpputils {

	// Combine multiple visitors into one
	// lifted from https://en.cppreference.com/w/cpp/utility/variant/visit example
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}
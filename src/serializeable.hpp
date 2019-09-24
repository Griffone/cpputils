#pragma once

#include <type_traits>

// Containers
#include <array>

#include <set>
#include <map>

#include <unordered_set>
#include <unordered_map>

#include <vector>
#include <deque>
#include <forward_list>
#include <list>

namespace cpputils {

	// CRTP marker for trivially-serializeable types
	// A trivially serializeable doesn't require anything more than mem-copying of it
	template<class T = void>
	struct trivially_serializeable {};

	// CRTP base for a non-trivial serializeable type
	// Any derivative class requires a serialize<Serializer>(Serializer& serializer) function definition that defines the order of serializations
	template<class T = void>
	struct Serializeable {};

	// Can an object be serialized by simply memcopying it?
	template<class T>
	inline constexpr bool is_trivially_serializeable_v = std::is_base_of_v<trivially_serializeable<T>, T> || std::is_arithmetic_v<T> || std::is_enum_v<T>;
	template<class T>
	struct is_trivially_serializeable : public std::bool_constant<is_trivially_serializeable_v<T>> {};

	// Can an object be serialized in linear time?
	// Examples: dynamic structures of trivially-serializeable types
	template<class T>
	inline constexpr bool is_simply_serializeable_v = is_trivially_serializeable_v<T>;
	template<class T>
	struct is_simply_serializeable : public std::bool_constant<is_simply_serializeable_v<T>> {};

	// Can an object be serialized without invoking object member serialize() function?
	// Examples: chains of containers of trivially-serializeable types
	template<class T>
	inline constexpr bool is_directly_serializeable_v = is_simply_serializeable_v<T>;
	template<class T>
	struct is_directly_serializeable : public std::bool_constant<is_directly_serializeable_v<T>> {};

	// Can an object be serialized?
	// Any trivially-serializeable, simply-serializeable, derivatives of Serializeable or linked conteinters of thereof are serializeable
	template<class T>
	inline constexpr bool is_serialzieable_v = is_simply_serializeable_v<T>;
	template<class T>
	struct is_serializeable : public std::bool_constant<is_serialzieable_v<T>> {};


	// Specializations

	template<class T, size_t N>
	inline constexpr bool is_trivially_serializeable_v<T[N]> = is_trivially_serializeable_v<T>;
	template<class T, size_t N>
	inline constexpr bool is_trivially_serializeable_v<std::array<T, N>> = is_trivially_serializeable_v<T>;


	template<class T, class Allocator>
	inline constexpr bool is_simply_serializeable_v<std::vector<T, Allocator>> = is_trivially_serializeable_v<T>;


	template<class T, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::vector<T, Allocator>> = is_directly_serializeable_v<T>;
	template<class T, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::deque<T, Allocator>> = is_directly_serializeable_v<T>;
	template<class T, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::forward_list<T, Allocator>> = is_directly_serializeable_v<T>;
	template<class T, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::list<T, Allocator>> = is_directly_serializeable_v<T>;

	template<class T, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::set<T, Compare, Allocator>> = is_directly_serializeable_v<T>;
	template<class T, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::multiset<T, Compare, Allocator>> = is_directly_serializeable_v<T>;
	template<class T, class Hash, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::unordered_set<T, Hash, Compare, Allocator>> = is_directly_serializeable_v<T>;
	template<class T, class Hash, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::unordered_multiset<T, Hash, Compare, Allocator>> = is_directly_serializeable_v<T>;

	template<class K, class V, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::map<K, V, Compare, Allocator>> = is_directly_serializeable_v<K> && is_directly_serializeable_v<V>;
	template<class K, class V, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::multimap<K, V, Compare, Allocator>> = is_directly_serializeable_v<K> && is_directly_serializeable_v<V>;
	template<class K, class V, class Hash, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::unordered_map<K, V, Hash, Compare, Allocator>> = is_directly_serializeable_v<K> && is_directly_serializeable_v<V>;
	template<class K, class V, class Hash, class Compare, class Allocator>
	inline constexpr bool is_directly_serializeable_v<std::unordered_multimap<K, V, Hash, Compare, Allocator>> = is_directly_serializeable_v<K> && is_directly_serializeable_v<V>;
}
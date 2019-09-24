#pragma once

#include "serializeable.hpp"

#include <istream>
#include <ostream>

namespace cpputils {

	class InputSerializer {
	public:
		InputSerializer(std::istream& stream) : stream(&stream) {};

		template<class T>
		void serialize(T& item) {
			static_assert(is_trivially_serializeable_v<T> || std::is_base_of_v<Serializeable<T>, T>, "Can not serialize item of given type!");

			if constexpr (is_trivially_serializeable_v<T>)
				stream->read(reinterpret_cast<char*>(&item), sizeof(T));
			else
				item.serialize(*this);
		}

		// Specializations

		template<class T, class Allocator>
		void serialize(std::vector<T, Allocator>& item) {
			static_assert(is_serialzieable_v<T>, "Can not serialize item of given type!");

			size_t size = 0;
			serialize(size);
			item.resize(size);
			if constexpr (is_trivially_serializeable_v<T>)
				stream->read(reinterpret_cast<char*>(item.data()), sizeof(T) * size);
			else {
				for (auto& member : item)
					serialize(member);
			}
		}

		// TODO: do other specializations

	private:
		std::istream* stream;
	};

	class OutputSerializer {
	public:
		OutputSerializer(std::ostream& stream) : stream(&stream) {};
		~OutputSerializer() { stream->flush(); }

		template<class T>
		void serialize(T& item) {
			static_assert(is_trivially_serializeable_v<T> || std::is_base_of_v<Serializeable<T>, T>, "Can not serialize item of given type!");

			if constexpr (is_trivially_serializeable_v<T>)
				stream->write(reinterpret_cast<char*>(&item), sizeof(T));
			else
				item.serialize(*this);
		}

		// Specializations

		template<class T, class Allocator>
		void serialize(std::vector<T, Allocator>& item) {
			static_assert(is_serialzieable_v<T>, "Can not serialize item of given type!");

			size_t size = item.size();
			serialize(size);
			if constexpr (is_trivially_serializeable_v<T>)
				stream->write(reinterpret_cast<char*>(item.data()), sizeof(T) * size);
			else {
				for (auto& member : item)
					serialize(member);
			}
		}

	private:
		std::ostream* stream;
	};
}
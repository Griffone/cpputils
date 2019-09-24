#include "boundedpolymorphism.hpp"
#include "overloaded.hpp"
#include "serializers.hpp"

#include <fstream>
#include <iostream>

// ===============================================================================================
// === Forward declarations of showcase functions
// ===============================================================================================

void showcase_bounded_polymorphism();
void showcase_vector_serialization();
void showcase_serializeable_types();


// ===============================================================================================
// === The showcase main function
// ===============================================================================================

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	showcase_bounded_polymorphism();
	showcase_vector_serialization();
	showcase_serializeable_types();

	return 0;
}

// ===============================================================================================
// === Helper structs and classes
// ===============================================================================================

class PolymorphismBase {
public:
	virtual size_t size() const { return 0; }
};

class SizeOne : public PolymorphismBase {
	size_t size() const override { return 1; }
};

class DynamicSize : public PolymorphismBase {
	size_t dynamicSize;
public:
	DynamicSize(size_t size) : dynamicSize(size) {}
	size_t size() const override { return dynamicSize; }
};

// Don't forget to mark CRTP
struct TriviallySerializeable : public cpputils::trivially_serializeable<TriviallySerializeable> {
	int x, y, z;

	TriviallySerializeable(int x, int y, int z) : x(x), y(y), z(z) {}
};

// Allow for inheritance with CRTP
template<class T = void>
struct SerializeableBase : public cpputils::Serializeable<SerializeableBase<T>> {
	TriviallySerializeable serializedData;
	size_t unserializedData = 0;
	size_t updatedOnSerialization = 0;

	SerializeableBase(TriviallySerializeable base) : serializedData(base) {}

	// The important bit
	template<class S>
	void serialize(S& serializer) {
		serializer.serialize(serializedData);
		++updatedOnSerialization;
	}
};

// Note that the marker doesn't get automatically forwarded due to CRTP.
// This is intended, as just becase a type inherits from a serializeable type it might not itself be serializeable.
struct SerializeableDeriv : public SerializeableBase<SerializeableDeriv>, public cpputils::Serializeable<SerializeableDeriv> {
	std::vector<int> moreSerializedData;

	SerializeableDeriv(TriviallySerializeable base, std::vector<int> more) : SerializeableBase<SerializeableDeriv>(base), moreSerializedData(more) {}

	template<class S>
	void serialize(S& serializer) {
		// Invoke the base method
		static_cast<SerializeableBase<SerializeableDeriv>*>(this)->serialize(serializer);
		serializer.serialize(moreSerializedData);
	}
};

template<class T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& data) {
	s << "{ ";
	for (const auto& item : data)
		s << item << ", ";
	return s << "}";
}
std::ostream& operator<<(std::ostream& s, const TriviallySerializeable& data) { return s << "{ x:" << data.x << ", y:" << data.y << ", z:" << data.z << " }"; }
template<class T>
std::ostream& operator<<(std::ostream& s, const SerializeableBase<T>& data) {
	s << "serializedData:         " << data.serializedData << '\n';
	s << "unserializedData:       " << data.unserializedData << '\n';
	s << "updatedOnSerialization: " << data.updatedOnSerialization << '\n';
	return s;
}
std::ostream& operator<<(std::ostream& s, const SerializeableDeriv& data) {
	s << static_cast<const SerializeableBase<SerializeableDeriv>&>(data);
	s << "moreSerializedData:     " << data.moreSerializedData << '\n';
	return s;
}

// ===============================================================================================
// === Bodies of showcase functions
// ===============================================================================================

void showcase_bounded_polymorphism() {
	std::cout << "\nBounded polymorphism:\n";
	using SizedObject = cpputils::BoundedPolymorphism<PolymorphismBase, SizeOne, DynamicSize>;

	// Construct a vector of different sized objects
	std::vector<SizedObject> vector = { SizeOne{}, DynamicSize{12}, DynamicSize{0} };

	std::cout << "Constructed a vector with (" << vector.size() << ") sized objects\n";
	
	for (size_t i = 0; i < vector.size(); ++i) {
		const auto& obj = vector[i];

		std::cout << "vector[" << i << "] (";
		// All normal functions over std::variant can be used

		std::visit(cpputils::overloaded{
			[](const SizeOne&) { std::cout << "SizeOne"; },
			[](const DynamicSize&) { std::cout << "DynamicSize"; },
			}, obj);

		// Access Base class with -> operator
		std::cout << ") size() = " << obj->size() << '\n';
	}
}

void showcase_vector_serialization() {
	std::cout << "\nVector serialization:\n";
	// Nested vectors
	using vector = std::vector<std::vector<int>>;

	vector output = { { 0, 1, 2}, {}, { 42, 69} };
	std::cout << "Output:\n" << output;

	{
		std::ofstream stream("vectordemo.tmp");
		cpputils::OutputSerializer serializer(stream);

		serializer.serialize(output);
	}

	vector input = {};
	std::cout << "\nInput before serialization:\n" << input;

	{
		std::ifstream stream("vectordemo.tmp");
		cpputils::InputSerializer serializer(stream);

		serializer.serialize(input);
	}
	std::cout << "\nInput after serialization:\n" << input;

	if (output == input)
		std::cout << "\noutput == input\n";
	else
		std::cout << "\noutput != input\n";
}

void showcase_serializeable_types() {
	std::cout << "\nSerializeable types:\n";
	// Base version
	{
		SerializeableBase output = { { 0, 1, 2 } };
		SerializeableBase input = { { -1, -1, -1} };

		output.unserializedData = 1234;
		{
			std::ofstream stream("basedemo.tmp");
			cpputils::OutputSerializer serializer(stream);
			serializer.serialize(output);
		}
		std::cout << "Base output:\n" << output;

		input.unserializedData = 4949;
		input.updatedOnSerialization = 16;
		std::cout << "\nBase input before serialization:\n" << input;
		{
			std::ifstream stream("basedemo.tmp");
			cpputils::InputSerializer serializer(stream);
			serializer.serialize(input);
		}
		std::cout << "\nBase input after serialization:\n" << input;
	}

	// Derived class version
	{
		SerializeableDeriv output = { { 1, 1, 2 }, { 3, 5, 8 } };
		SerializeableDeriv input = { { 0, 0, 3 }, { 1, 4, 1, 5, 9, 2, 6, 5, 3, 5 } };

		{
			std::ofstream stream("basedemo.tmp");
			cpputils::OutputSerializer serializer(stream);
			serializer.serialize(output);
		}
		std::cout << "\nDerived output:\n" << output;

		std::cout << "\nDerived input before serialization:\n" << input;
		{
			std::ifstream stream("basedemo.tmp");
			cpputils::InputSerializer serializer(stream);
			serializer.serialize(input);
		}
		std::cout << "\nDerived input after serialization:\n" << input;
	}
}
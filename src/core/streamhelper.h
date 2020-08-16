#pragma once
#include <core.h>
#include <cstdint>
#include <iostream>
#include <vector>

namespace detail {

	/**
	 * Static assert helper for enforcing POD types while being very out of the way.
	 * 
	 * Forward-compatible with C++20's deprecation of is_pod<T>, 
	 * using is_standard_layout<T> in cases where C++20 experimental support is enabled.
	 *
	 * \tparam T The type that this object should assert is a POD type during compile time.
	 */
	template<class T>
	struct PodAssert {
		#if __cplusplus >= 201704L
			static_assert(std::is_standard_layout<T>::value, "Non-POD (Plain Old Data) type detected.");
		#else
			static_assert(std::is_pod<T>::value, "Non-POD (Plain Old Data) type detected. (C++20)");
		#endif
	};

}

namespace xb2at {
namespace core {

	/**
	 * A helper object making reading POD (Plain Old Data) types from a C++ iostream object easier.
	 * Essentially like a BinaryReader.. that can read any type you want *as well as* type primitives!
	 * Cool, huh?
	 */
	struct StreamHelper {
	
		StreamHelper(std::istream& stream) 
			: stream(stream) { 
			
		}
			
		// return the stream this StreamHelper is managing
		inline std::istream& raw() { return stream; } 
			
		/**
		 * Read any POD type.
		 * You can use this to, for instance,
		 * read a header in one shot instead of reading in 
		 * *each* and every member.
		 * Validation and checking is still up to you, though.
		 *
		 * \tparam T Type to read. Must be POD.
		 * \param[in] data The struct to read into.
		 */
		template<class T>
		inline bool ReadType(T& data) {

			// The PodAssert object is only relevant during compile time (as it's only used to 
			// so make it constexpr and unused
			constexpr CORE_UNUSED detail::PodAssert<T> pod{};
			
			if(!stream)
				return false;
			
			stream.read((char*)&data, sizeof(T));
			return true;
		}

		/**
		 * Read any POD type (returning it instead of returning into a reference).
		 * You can use this to, for instance, read a header in one shot instead of reading in *each* and every member.
		 * Validation and checking is still up to you, though.
		 * There is also NO safety checking with this overload. 
		 * You'll have to check the stream is still good before calling this.
		 *
		 * \tparam T Type to read. Must be POD.
		 */
		template<class T>
		inline T ReadType() {
			constexpr CORE_UNUSED detail::PodAssert<T> pod{};
			T temp{};

			if(!stream)
				return temp;
			
			stream.read((char*)&temp, sizeof(T));
			return true;
		}
			
		/**
		 * Read a string, stopping when a NUL is reached.
		 */
		inline std::string ReadString() {
			std::string str;
						
			while(true) {
				char c = stream.get();
				if(c == '\0')
					break;
	
				str += c;
			}
			
			return str;
		}
		
		/** 
		 * Read a string, stopping at the specified length.
		 * Really only useful if you're absoultely sure that the string has a fixed size.
		 * Otherwise just use the overload that takes no arguments.
		 *
		 * \param[in] length Length of the string.
		 */
		inline std::string ReadString(std::size_t length) {
			std::string str;
			str.resize(length);
			
			stream.read(str.data(), length);
			return str;
		}

		/**
		 * Read an array of T.
		 *
		 * \tparam T Type to read array of.
		 * \param[in] length Length of the array.
		 */
		template<class T>
		inline std::vector<T> ReadArray(std::size_t length) {
			std::vector<T> array;
			
			for(int i = 0; i < length; ++i) {
				T item;

				if(!ReadType<T>(item))
					break;

				array.push_back(item);
			}
			
			return array;
		}
		
		/**
		 * Read a Vector3 comprised of floats.
		 * Returns said Vector3.
		 */
		inline vector3 ReadVec3() {
			float x;
			float y;
			float z;
			
			ReadType<float>(x);
			ReadType<float>(y);
			ReadType<float>(z);

			return {x, y, z};
		}

		
		/**
		 * Read a Vector2 comprised of floats.
		 * Returns said Vector2.
		 */
		inline vector2 ReadVec2() {
			float x;
			float y;
			
			ReadType<float>(x);
			ReadType<float>(y);

			return {x, y};
		}

		/**
		 * Read a quaternion comprised of floats.
		 * Returns said quaternion.
		 */
		inline quaternion ReadQuaternion() { 
			float x;
			float y;
			float z;
			float w;
			
			ReadType<float>(x);
			ReadType<float>(y);
			ReadType<float>(z);
			ReadType<float>(w);
			
			return {x, y, z, w};
		}

		/**
		 * Read a quaternion comprised of 4 signed 8-bit values.
		 * Returns said quaternion.
		 */
		inline quaternion ReadS8Quaternion() {
			uint32 total;
			ReadType<uint32>(total);

			float x = (float) ( ((sbyte*)&total)[0] ) / 128.f;
			float y = (float) ( ((sbyte*)&total)[1] ) / 128.f;
			float z = (float) ( ((sbyte*)&total)[2] ) / 128.f;
			float w = (float) ( ((sbyte*)&total)[3] );

			return { x, y, z, w };
		}

		/**
		 * Read a quaternion comprised of 4 unsigned 8-bit values.
		 * Returns said quaternion.
		 */
		inline quaternion ReadU8Quaternion() {
			uint32 total;
			ReadType<uint32>(total);

			float x = (float) ( ((byte*)&total)[0] ) / 128.f;
			float y = (float) ( ((byte*)&total)[1] ) / 128.f;
			float z = (float) ( ((byte*)&total)[2] ) / 128.f;
			float w = (float) ( ((byte*)&total)[3] );

			return { x, y, z, w };
		}

		/**
		 * Read a quaternion comprised of 4 unsigned 16-bit values.
		 * Returns said quaternion.
		 */
		inline quaternion ReadU16Quaternion() { 
			uint64 total;
			ReadType<uint64>(total);

			float x = (float) ( ((uint16*)&total)[0] ) / 65535.f;
			float y = (float) ( ((uint16*)&total)[1] ) / 65535.f;
			float z = (float) ( ((uint16*)&total)[2] ) / 65535.f;
			float w = (float) ( ((uint16*)&total)[3] ) / 65535.f;
			
			return {x, y, z, w};
		}

	private:
		/** 
		 * The stream this StreamHelper object is managing.
		 */
		std::istream& stream;
	};

}
}
#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

namespace detail {

	/**
	 * Static assert helper for enforcing POD types while not being (very) ugly.
	 * Forward-compatible with C++20's deprecation of is_pod<T>, 
	 * using is_standard_layout<T> in cases where C++20 experimental support is enabled.
	 *
	 * \tparam T The type that this object should assert is a POD type.
	 */
	template<class T>
	struct PodAssert {
		#if __cplusplus >= 201704L
			static_assert(std::is_standard_layout<T>::value, "The given type is not a Plain Old Data (POD) type.");
		#else
			static_assert(std::is_pod<T>::value, "The given type is not a Plain Old Data (POD) type.");
		#endif
	};

}

namespace xb2at {
namespace core {

	/**
	 * A helper object making reading POD (Plain Old Data) types from a C++ iostream object easier.
	 * Essentially a BinaryReader on crack
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
			static detail::PodAssert<T> pod;
			
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
			static detail::PodAssert<T> pod;
			T temp;

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

	private:
		/** 
		 * The stream this StreamHelper object is managing.
		 */
		std::istream& stream;
	};

}
}
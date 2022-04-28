//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_IOSTREAMREADSTREAM_H
#define XB2AT_IOSTREAMREADSTREAM_H

#include <xb2at/core/Stream.h>

#include <bit>
#include <iostream>
#include <array>
#include <vector>

#include <xb2at/core/EndianUtils.h>

namespace xb2at::core {

	/**
	 * A Stream which wraps a IOStreams input stream for reading.
	 */
	struct IoStreamReadStream {
		using IsStream = void; // this class is in fact a Stream

		explicit IoStreamReadStream(std::istream& stream)
			: stream(stream) {
		}

		/**
		 * Trait function, returns whether or not this is a read stream at compile time
		 */
		consteval static bool IsReadStream() {
			return true;
		}


		// TODO: .cpp

		inline std::size_t Tell() {
			return GetStream().tellg();
		}

		inline void Seek(StreamSeekDir dir, std::size_t offset) {
			std::istream::seekdir pos;
			switch(dir) {
				case StreamSeekDir::Begin:
					pos = std::istream::beg;
				case StreamSeekDir::Current:
					pos = std::istream::cur;
				case StreamSeekDir::End:
					pos = std::istream::end;
			}
			GetStream().seekg(offset, pos);
		}

		bool Byte(std::uint8_t& b);

		// This template is written once and expanded.
#define TYPE(methodName, T)    \
	template<std::endian Endian>     \
	inline bool methodName(T& t) {   \
		return ReadThing<Endian>(t); \
	}
#include <xb2at/core/StreamTypeListing.inl>
#undef TYPE

		template<std::size_t N>
		inline bool FixedSizeArray(std::uint8_t (&arr)[N]) {
			if(!stream)
				return false;

			for(std::size_t i = 0; i < N; ++i)
				if(!this->Byte(arr[i]))
					return false;

			return true;
		}

		template<std::size_t N>
		inline bool FixedString(char (&fixedStr)[N]) {
			if(!stream)
				return false;

			for(std::size_t i = 0; i < N; ++i)
				if(!this->Byte(reinterpret_cast<std::uint8_t&>(fixedStr[i])))
					return false;

			return true;
		}

		bool String(std::string& string);

		/**
		 * Get the raw stream this is wrapping.
		 */
		[[nodiscard]] std::istream& GetStream() const;

		/**
		 * Helper
		 */
		template<std::endian Endian, class T>
		inline bool GivenType(T& t) {
#define TYPE(func, U) \
        if constexpr(std::is_same_v<T, U>) \
				if(!this->template func<Endian>(t))\
					return false;
#include <xb2at/core/StreamTypeListing.inl>
#undef TYPE

			// TODO: add strings, and FixedByteArray/FixedString

			return true;
	    }


		template<std::endian Endian, class T>
		inline bool Array(std::size_t Count, std::vector<T>& vec) {
			vec.resize(Count);

			for(auto& item : vec)
				if(!this->template GivenType<Endian, T>(item))
					return false;

			return true;
		}


		template<class T>
		inline bool Other(T& t) {
			return t.Transform(*this);
		}

	   private:
		/**
		 * Internal helper for most types.
		 */
		template<std::endian Endian, class T>
		inline bool ReadThing(T& t) {
			if(!stream)
				return false;

			// This buffer is used to read the swappable object in.
			std::uint8_t readin_buffer[sizeof(T)];
			stream.read(reinterpret_cast<char*>(&readin_buffer[0]), sizeof(T));

			// This will reinterpret the object properly with endian (temporarily copying it, however it's on the stack so no big deal).
			t = core::ReadEndian<Endian, T>(&readin_buffer[0]);
			return true;
		}

		/**
		 * The backing stream.
		 */
		std::istream& stream;
	};

}
#endif //XB2AT_IOSTREAMREADSTREAM_H

//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_STREAM_H
#define XB2AT_STREAM_H

#include <concepts>
#include <bit> // Uint* methods use std::endian, so include it here..

namespace xb2at::core {

	/**
	 * Stream seek direction.
	 */
	enum class StreamSeekDir : std::uint8_t {
		Begin,
		Current,
		End
	};


	/**
	 * Constrains a template function to working with streams.
	 */
	template<class T>
	concept Stream = requires(T stream) {
		typename T::IsStream;

		// This is a consteval static trait function which returns true
		// if the stream is reading.
		{ T::IsReadStream() } -> std::same_as<bool>;
		{ stream.Seek(StreamSeekDir::Begin, 1) } -> std::same_as<void>;
	};

	/**
	 * Handy macro to avoid writing the boilerplate to
	 * catch an error during transformation.
	 */
#define XB2AT_TRANSFORM_CATCH(x) if(!(x)) return false

}

#endif //XB2AT_STREAM_H

//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_AUTOREWIND_H
#define XB2AT_AUTOREWIND_H

#include <iostream>
#include <cstdint>

namespace xb2at::core {

	/**
	 * A helper to automatically rewind a stream to pre-execution offset.
	 */
	struct AutoRewindStream {
		inline AutoRewindStream(std::istream& is)
			: stream(is) {
			last_offset = stream.tellg();
		}

		inline ~AutoRewindStream() {
			stream.seekg(last_offset, std::istream::beg);
		}

	   private:
		std::istream& stream;
		std::size_t last_offset;
	};


}

#endif //XB2AT_AUTOREWIND_H

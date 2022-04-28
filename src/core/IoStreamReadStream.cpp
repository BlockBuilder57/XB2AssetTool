#include <xb2at/core/IoStreamReadStream.h>

namespace xb2at::core {


	bool IoStreamReadStream::Byte(std::uint8_t& b) {

		if(!stream.get(reinterpret_cast<char&>(b)))
			return false;
		return true;

		//if(stream.good())
		//	b = stream.get();
		//else
		//	return false;
		//return true;
	}


	bool IoStreamReadStream::String(std::string& string) {
		if(!stream)
			return false;

		// This isn't terribly performant, but there really isn't much that needs
		// string reading, so it's fine.
		char c;
		while(true) {
			if(!stream.get(c))
				return false;

			if(c == '\0')
				break;

			string += c;
		}
		return true;
	}

	/**
	 * Get the raw stream this is wrapping.
	 */
	std::istream& IoStreamReadStream::GetStream() const {
		return stream;
	}

	// This pre-instantiates all of the stream methods, so they're free to use for anything.
	// Anything not used will get linked out when built as Release anyways.
#define TYPE(FuncName, T) \
	template bool IoStreamReadStream::FuncName<std::endian::little>(T&); \
	template bool IoStreamReadStream::FuncName<std::endian::big>(T&);
#include <xb2at/core/StreamTypeListing.inl>
#undef TYPE
}
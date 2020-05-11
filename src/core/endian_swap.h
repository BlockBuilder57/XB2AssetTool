#pragma once
#include <core.h>

#ifdef _MSC_VER
	#include <intrin.h>
	// Prefer the intrinsic function versions
	// of the following functions, if they
	// exist on the target. These will be similar to GCC
	// intrinisics (we never call into the CRT anyways, so, it should still be relatively quick)
	#pragma intrinsic(_byteswap_ushort)
	#pragma intrinsic(_byteswap_ulong)
	#pragma intrinsic(_byteswap_uint64)

	#define BYTESWAP16(x) _byteswap_ushort(x)
	#define BYTESWAP32(x) _byteswap_ulong(x)
	#define BYTESWAP64(x) _byteswap_uint64(x)
#elif defined(__GNUC__)
	// Builtin functions with GNU C get turned into (sometimes single-instruction) intrinisics
	// usually by default if the target supports them. Otherwise,
	// they become inline functions (which still *have* a speed penalty, 
	// but far less then if it had to make a call into the C runtime)
	#define BYTESWAP16(x) __builtin_bswap16(x)
	#define BYTESWAP32(x) __builtin_bswap32(x)
	#define BYTESWAP64(x) __builtin_bswap64(x)
#else
#error Unsupported compiler.
#endif

namespace xb2at {
namespace core {
	
	/**
	 * Swap the endian of a provided value.
	 *
	 * \tparam T Type
	 * \param[in] value value to swap endian of
	 */
	template<typename T>
	inline T swap_endian(T value) {
		if constexpr(sizeof(T) == 2) {
			return BYTESWAP16(value);
		} else if constexpr(sizeof(T) == 4) {
			return BYTESWAP32(value);
		} else if constexpr(sizeof(T) == 8) {
			return BYTESWAP64(value);
		} else {
			// swap sizeof(std::uint16_t) bytes at a time of any structure
			T temp{};
			memcpy(&temp, &value, sizeof(T));

			for(int32 i = 0; i < sizeof(T); i += sizeof(std::uint16_t))
				((std::uint16_t*)&temp)[i] = BYTESWAP16(((std::uint16_t*)&temp)[i]);

			return temp;
		}
	}

}
}

#undef BYTESWAP16
#undef BYTESWAP32
#undef BYTESWAP64
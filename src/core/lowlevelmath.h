/**
 * \file
 * Low level inline/constexpr math routines
 */

#pragma once
#include <core.h>

// Because we use compiler dependent builtins
// we need to switch between them with ifdefs
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
		 * Swap the endian of a provided trivial value.
		 *
		 * \tparam T Type of trivial value.
		 * \param[in] value value to swap endian of
		 */
		template<typename T, typename = typename std::enable_if<std::is_trivial_v<T>, T>::type /* SFINAE to avoid non-trivial usage */>
		constexpr T swap_endian(T value) {
			if constexpr(sizeof(T) == 2) {
				return BYTESWAP16(value);
			} else if constexpr(sizeof(T) == 4) {
				return BYTESWAP32(value);
			} else if constexpr(sizeof(T) == 8) {
				return BYTESWAP64(value);
			} else {
				// swap sizeof(std::uint16_t) bytes at a time of any trivial structure
				T temp {};

				// copy value structure to temporary one
				memcpy(&temp, &value, sizeof(T));

				// then do the magic here by treating it as an array of uint16's
				for(int32 i = 0; i < sizeof(T); i += sizeof(std::uint16_t))
					((std::uint16_t*)&temp)[i] = BYTESWAP16(((std::uint16_t*)&temp)[i]);

				return temp;
			}
		}

		/**
		 * Access a bit in a trivial value.
		 * 
		 * \tparam T type
		 * \param[in] v Value
		 * \param[in] index Index/bit to access.
		 */
		template<typename T, typename = typename std::enable_if<std::is_trivial_v<T>, T>::type /* SFINAE to avoid non-trivial usage */>
		constexpr bool access_bit(T& v, std::size_t index) {
			return (v & (1 << index));
		}

		/**
		 * Count zero bits in a trivial value.
		 * 
		 * \tparam T type
		 * \param[in] v Value
		 */
		template<typename T, typename = typename std::enable_if<std::is_trivial_v<T>, T>::type /* SFINAE to avoid non-trivial usage */>
		constexpr int32 count_zeros(const T& v) {
			for(std::size_t i = 0; i < sizeof(T); ++i) {
				if(access_bit(v, i) != 0)
					return i;
			}

			return sizeof(T);
		}

	} // namespace core
} // namespace xb2at

// Undefine our internal portability macros
// as to not clutter any files which include this one.
#undef BYTESWAP16
#undef BYTESWAP32
#undef BYTESWAP64
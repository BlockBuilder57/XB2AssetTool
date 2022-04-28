//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_ENDIANUTILS_H
#define XB2AT_ENDIANUTILS_H

#include <bit>
#include <climits>
#include <cstdint>

// In this case, the compiler detection in this file is intended
// to condense intrinsics into a single macro, so that the code
// is more readable, and does not have lots of #ifdef garbage.
// Basically, a "get the ugliness done and over with so we can replace it with better code" strategy.

#ifdef _MSC_VER // MSVC
	#include <intrin.h>

	// Prefer the intrinsic function versions of the following functions, if available
	#pragma intrinsic(_byteswap_ushort)
	#pragma intrinsic(_byteswap_ulong)
	#pragma intrinsic(_byteswap_uint64)

	#define XB2AT_BYTESWAP16(x) _byteswap_ushort(x)
	#define XB2AT_BYTESWAP32(x) _byteswap_ulong(x)
	#define XB2AT_BYTESWAP64(x) _byteswap_uint64(x)
#else
	#ifdef __GNUC__ // GCC & Clang (including Emscripten)
		// Builtin functions with GNU C & Clang get turned into (sometimes single-instruction) intrinsics
		// usually by default if the target supports them.
		// Otherwise, they become inline functions
		// (which still *have* a speed penalty, but far less then if it had to make a call into the C runtime)
		#define XB2AT_BYTESWAP16(x) __builtin_bswap16(x)
		#define XB2AT_BYTESWAP32(x) __builtin_bswap32(x)
		#define XB2AT_BYTESWAP64(x) __builtin_bswap64(x)
	#else
		#error Unsupported compiler. Check in support for it?
	#endif
#endif

namespace xb2at::core {
	/**
	 * Concept constraining a type which is byte-swappable.
	 * This type must be:
	 *  - Bigger than or equal to sizeof(std::uint16_t)...
	 *  - BUT smaller than or equal to sizeof(std::uint64_t)
	 *  - It must also be a "trivial" type i.e std::is_trivial_v<T> must be true
	 */
	template<class T>
	concept IsSwappable = (sizeof(T) >= sizeof(std::uint16_t)) && (sizeof(T) <= sizeof(std::uint64_t)) && std::is_trivial_v<T>;

	namespace detail {

		/**
		 * Do a swap of a thing of type T.
		 */
		template<class T>
		constexpr T Swap(const T& val) requires(IsSwappable<T>) {
			switch(sizeof(T)) {
				case sizeof(std::uint16_t):
					return XB2AT_BYTESWAP16(val);

				case sizeof(std::uint32_t):
					return XB2AT_BYTESWAP32(val);

				case sizeof(std::uint64_t):
					return XB2AT_BYTESWAP64(val);

					// TODO: A worst-case path which swaps
					//  sizeof(std::uint16_t) aligned types.
					//  Once applied here, code will uniformly support this right away.
			}
		}

// Once this function is done these macros are NOT needed anymore
#undef XB2AT_BYTESWAP16
#undef XB2AT_BYTESWAP32
#undef XB2AT_BYTESWAP64

		/**
		 * Perform an endian swap if the host compile endian
		 * is equal to the Endian template parameter. This is intended
		 * to be used with the OppositeEndian trait.
		 */
		template<std::endian Endian, class T>
		inline T SwapIfEndian(const T& val) requires(IsSwappable<T>) {
			if constexpr(std::endian::native == Endian)
				return Swap(val);
			return val;
		}

		/**
		 * Get a reference as T to a buffer.
		 */
		template<class T>
		constexpr T& PointerTo(void* ptr) {
			return *static_cast<T*>(ptr);
		}

		/**
		 * Get a const reference as T to a const buffer.
		 *
		 * Is exactly the same as the above PointerTo, however
		 * only participates in overload resolution if T is const.
		 */
		template<class T>
		constexpr T& PointerTo(const void* ptr) requires(std::is_const_v<T>) {
			return *static_cast<T*>(ptr);
		}

		consteval std::endian OppositeEndian(std::endian e) {
			switch(e) {
				case std::endian::little:
					return std::endian::big;
				case std::endian::big:
					return std::endian::little;
			}
		}

	} // namespace detail

	template<std::endian Endian, class T>
	std::remove_cvref_t<T> ReadEndian(const std::uint8_t* base) requires(IsSwappable<std::remove_cvref_t<T>>) {
		const auto& ref = detail::PointerTo<const std::remove_cvref_t<T>>(base);
		return detail::SwapIfEndian<detail::OppositeEndian(Endian), std::remove_cvref_t<T>>(ref);
	}

	template<std::endian Endian, class T>
	void WriteEndian(std::uint8_t* base, const std::remove_cvref_t<T>& val) requires(IsSwappable<std::remove_cvref_t<T>>) {
		auto& i = detail::PointerTo<std::remove_cvref_t<T>>(base);
		i = detail::SwapIfEndian<detail::OppositeEndian(Endian), std::remove_cvref_t<T>>(val);
	}

} // namespace ssxtools::core

#endif //XB2AT_ENDIANUTILS_H

/**
 * \file
 * Low level inline/constexpr math routines
 */

#pragma once
#include <type_traits>

namespace xb2at::core {

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
	constexpr std::size_t count_zeros(const T& v) {
		for(std::size_t i = 0; i < sizeof(T) * CHAR_BIT; ++i) {
			if(access_bit(v, i) != 0)
				return i;
		}

		return sizeof(T) * CHAR_BIT;
	}

} // namespace xb2at::core

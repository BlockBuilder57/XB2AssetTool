//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_UNDERLYINGVALUE_H
#define XB2AT_UNDERLYINGVALUE_H

#include <type_traits>

namespace xb2at::core {

	namespace detail {
		template<class T>
		concept Enum = std::is_enum_v<T>;
	}

	/**
	 * Returns a reference of the value as the underlying type of the enum
	 */
	template<detail::Enum T>
	inline std::underlying_type_t<T>& UnderlyingValue(T& t) {
		return reinterpret_cast<std::underlying_type<T>&>(t);
	}

	template<detail::Enum T>
	constexpr const std::underlying_type_t<T>& UnderlyingValue(const T& t) {
		return reinterpret_cast<const std::underlying_type<T>&>(t);
	}

}

#endif //XB2AT_UNDERLYINGVALUE_H

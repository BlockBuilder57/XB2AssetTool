//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_OBJECTAS_H
#define XB2AT_OBJECTAS_H

#include <type_traits>

namespace xb2at::core {

	/**
	 * Returns a reference to one of an object's base classes
	 */
	template<class T, class U>
	inline T& ObjectAs(U& u) {
		static_assert(std::is_base_of_v<T, U>, "U doesn't inherit from T");
		return reinterpret_cast<T&>(u);
	}

	/**
	 * \copydoc ObjectAs()
	 */
	template<class T, class U>
	inline const T& ObjectAs(const U& u) {
		static_assert(std::is_base_of_v<T, U>, "U doesn't inherit from T");
		return reinterpret_cast<T&>(u);
	}

}

#endif //XB2AT_OBJECTAS_H

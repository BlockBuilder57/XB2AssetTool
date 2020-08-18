/**
 * \file 
 * Core library common include file.
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <future>
#include <functional>
#include <algorithm>

#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace xb2at {
namespace core {

	
	/**
	 * \defgroup Macros Core Library Macro Definitions
	 * @{
	 */

#if defined(__clang__) || defined(__GNUC__)
	/**
	 * Marks a value as unused so that with tighter warning settings it doesn't emit warnings.
	 */
	#define CORE_UNUSED __attribute__((unused))
#else
	// no-op for compilers that are looser about it
	#define CORE_UNUSED
#endif

	/** @} */

	/**
	 * \defgroup Types Core Library Types
	 * @{
	 */
	
	/**
	 * Unsigned 8-bit value.
	 */
	typedef std::uint8_t byte;

	/**
	 * Signed 8-bit value.
	 */
	typedef std::int8_t sbyte;

	typedef std::int16_t int16;
	typedef std::uint16_t uint16;

	typedef std::int32_t int32;
	typedef std::uint32_t uint32;

	typedef std::int64_t int64;
	typedef std::uint64_t uint64;

	/**
	 * A bool that is not sizeof(int) bytes.
	 */
	typedef byte bytebool;
	
	/**
	 * constant shorthand for numeric_limits<T>::min()
	 */
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	struct Min { constexpr static T value = std::numeric_limits<T>::min(); };

	/**
	 * constant shorthand for numeric_limits<T>::max()
	 */
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	struct Max { constexpr static T value = std::numeric_limits<T>::max(); };
	
	/**
	 * Vector2 of floats.
	 */
	struct vector2 {
		float x;
		float y;
	};

	/**
	 * Vector3 of floats.
	 */
	struct vector3 {
		float x;
		float y;
		float z;
	};

	
	/**
	 * Quaternion of floats.
	 */
	struct quaternion {
		float x;
		float y;
		float z;
		float w;
	};

	struct u16_quaternion {
		uint16 x;
		uint16 y;
		uint16 z;
		uint16 w;
	};

	struct color {
		byte r;
		byte g;
		byte b;
		byte a;
	};

	struct matrix4x4 {
		float m1,  m2,  m3,  m4;
		float m5,  m6,  m7,  m8;
		float m9,  m10, m11, m12;
		float m13, m14, m15, m16;
	};

	/**
	 * Like std::map<Key, Value> but in compile time! 
	 */
	template<class Key, class Value, std::size_t Size>
	struct CompileTimeMap {
		
		/**
		 * Find a value in the map.
		 * This search can be done during compile time,
		 * and if so, the value will actually be replaced with just the applicable 
		 * value *at* compile time!
		 */
		[[nodiscard]]
		constexpr Value at(const Key& key) const {
			const auto it = std::find_if(std::begin(values), std::end(values), [&key](const auto& v) {
				return v.first == key;
			});

			if(it != std::end(values))
				return it->second;

			throw std::range_error("not found");
		}

		std::array<std::pair<Key, Value>, Size> values;
	};
	
	/** @} */

	// TODO(lily): Due to C++17 copy elision,
	// most of the functions here that return void but modify a reference to T
	// should be just fine returning T{}.

	/**
	 * Normalize a Vector3.
	 *
	 * \param[in] vector Vector to normalize.
	 */
	inline vector3 NormalizeVector3(vector3 vector) {
		const double mag = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
		return { 
			vector.x / (float)mag, 
			vector.y / (float)mag,
			vector.z / (float)mag 
		};
	}

	/**
	 * Convert a Vector3 position, a quaternion rotation, and a Vector3 scale to a 4x4 matrix.
	 *
	 * \param[in] pos Position of the object.
	 * \param[in] rot Rotation of the object.
	 * \param[in] scale Scale of the object.
	 */
	inline glm::mat4x4 MatrixGarbage(quaternion pos, quaternion rot, quaternion scale) {
		return glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z)) *   // position
			glm::toMat4(glm::quat(rot.w, rot.x, rot.y, rot.z)) *                   // rotation 
			glm::translate(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z)); // matrix scale
	}

	/**
	 * \defgroup FunTempls Core Library Function Templates
	 * @{
	 */

	/**
	 * Resize a multi-dimensional vector of elements of type T.
	 *
	 * \tparam T Element type
	 * 
	 * \param[in] vec Multi-dimensional vector to resize
	 * \param[in] dim1 First dimension
	 * \param[in] dim2 Second dimension
	 */
	template<class T>
	inline void ResizeMultiDimVec(std::vector<std::vector<T>>& vec, int dim1, int dim2) {
		vec.resize(dim1);

		for(std::vector<T>& v : vec)
			v.resize(dim2);
	}
	
	/**
	 * Default min function for GetMinElement().
	 *
	 * \tparam T Type to compare.
	 *
	 * \param[in] LValue Left value.
	 * \param[in] RValue Right value.
	 */
	template<typename T>
	inline bool DefaultMin(const T& LValue, const T& RValue) {
		return std::min(LValue, RValue) == RValue;
	}

	/**
	 * Default max function for GetMaxElement().
	 *
	 * \tparam T Type to compare.
	 *
	 * \param[in] LValue Left value.
	 * \param[in] RValue Right value.
	 */
	template<typename T>
	inline bool DefaultMax(const T& LValue, const T& RValue) {
		return std::max(LValue, RValue) == RValue;
	}
	
	/**
	 * Get the max element inside of a container.
	 *
	 * \tparam RevIterContainer Container type to use. Must follow ReversiableContainer.
	 * \tparam MaxFun Max function lambda type. Must be `bool (const T&, const T&)`.
	 *
	 * \param[in] container Container to use.
	 * \param[in] Max Max function to use. Refer to the documentation of the MaxFun template parameter.
	 */
	template<class RevIterContainer, class MaxFun>
	inline typename RevIterContainer::value_type GetMaxElement(const RevIterContainer& container, MaxFun& Max = DefaultMax<typename RevIterContainer::value_type>) {
		typedef typename RevIterContainer::value_type T;

		auto it = std::max_element(container.begin(), container.end(), [&](const T& L, const T& R) {
			return Max(L, R);
		});

		return (*it);
	}

	
	/**
	 * Get the min element inside of a container.
	 *
	 * \tparam RevIterContainer Container type to use. Must follow ReversiableContainer.
	 * \tparam MaxFun Max function lambda type. Must be bool (const T&, const T&).
	 *
	 * \param[in] vec Container to use.
	 * \param[in] Min Min function to use. Refer to the documentation of the MinFun template parameter.
	 */
	template<class RevIterContainer, class MinFun>
	inline typename RevIterContainer::value_type GetMinElement(const RevIterContainer& container, MinFun& Min = DefaultMin<typename RevIterContainer::value_type>) {
		typedef typename RevIterContainer::value_type T;

		auto it = std::min_element(vec.begin(), vec.end(), [&](const T& L, const T& R) {
			return Min(L, R);
		});

		return (*it);
	}

	/**
	 * Clamp a expression to specified min and max values.
	 *
	 * \tparam T Value type.
	 *
	 * \param[in] Expression Expression or value to clamp.
	 * \param[in] Min Minimum value.
	 * \param[in] Max Maximum value.
	 */
	template<typename T>
	inline T Clamp(T Expression, const T Min, const T Max) {
		return (Expression > Max) ? Max :
			(Expression < Min) ? Min : Expression;
	}

	/**
	 * Replacement for LINQ's .Where function.
	 * Takes a lambda function that returns a bool. If it returns true,
	 * the iterator to that object will be returned. 
	 * To get a applicable index, you can do `std::distance(container.begin(), it)`,
	 * where `it` can either be a previously gathered index or a Where() call.
	 *
	 * \tparam RevIterContainer Container type to use. Must follow ReversiableContainer.
	 * \tparam WhereFun Where lambda function. Must be `bool (const T&)`.
	 *
	 * \param[in] vec Container to find an item in.
	 * \param[in] evalulator Evalulator function. Refer to the documentation of the WhereFun template parameter.
	 */
	template<class RevIterContainer, class WhereFun>
	inline typename RevIterContainer::iterator Where(RevIterContainer& vec, WhereFun evalulator) {
		auto it = vec.begin();

		// return end iterator if container has no allocated elements
		if(vec.size() == 0)
			return vec.end();

		for(int i = 0; i < vec.size(); ++i) {
			if(evalulator(*it))
				return it;

			it++;
		}

		// return end iterator if the evalulator never returned true
		return vec.end();
	}

	/**
	 * Returns the element count of an array on the stack.
	 *
	 * \tparam T Type of the array.
	 * \tparam N Count of elements in the array.
	 */
	template<typename T, int N>
	constexpr std::size_t ArraySize(T(&)[N]) {
		return N;
	}

	/** @} */

	/**
	 * Namespace alias of the filesystem library, for shortening usage.
	 */
	namespace fs = std::filesystem;
}
}
/**
 * \file 
 * Core library common include file.
 */
#pragma once

#include <cstdint>
#include <modeco/types.h>

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
	#define CORE_UNUSED __attribute__((unused))
#else
	// no-op for compilers that are looser about it
	/**
	 * Marks a value as unused so that with tighter warning settings it doesn't emit warnings.
	 */
	#define CORE_UNUSED
#endif

		/** @} */

		/**
		 * \defgroup Types Core Library Types & Class Templates
		 * @{
		 */
		
		using mco::byte;
		using mco::sbyte;

		using mco::int16;
		using mco::uint16;
		
		using mco::int32;
		using mco::uint32;

		using mco::int64;
		using mco::uint64;

		/**
		 * constexpr shorthand for numeric_limits<T>::min()
		 */
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type /* SFINAE to stop invalid types */>
		struct Min { constexpr static T value = std::numeric_limits<T>::min(); };

		/**
		 * constexpr shorthand for numeric_limits<T>::max()
		 */
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type /* SFINAE to stop invalid types */>
		struct Max { constexpr static T value = std::numeric_limits<T>::max(); };
		
		/**
		 * STL-style _v alias for Min<T>
		 */
		template<typename T>
		constexpr static T Min_v = Min<T>::value;

		/**
		 * STL-style _v alias for Max<T>
		 */
		template<typename T>
		constexpr static T Max_v = Max<T>::value;

		/**
		 * Vector2 of floats, used only for reading into.
		 */
		struct vector2 {
			float x;
			float y;
		};

		/**
		 * Vector3 of floats, used only for reading into.
		 */
		struct vector3 {
			float x;
			float y;
			float z;
		};

		/**
		 * Quaternion of floats, used only for reading into.
		 */
		struct quaternion {
			float x;
			float y;
			float z;
			float w;
		};

		/**
		 * Quaternion of u16's, used only for reading into.
		 */
		struct u16_quaternion {
			uint16 x;
			uint16 y;
			uint16 z;
			uint16 w;
		};

		/**
		 * RGBA color
		 */
		struct color {
			byte r;
			byte g;
			byte b;
			byte a;
		};

		/** @} */

		/**
		 * \defgroup InlineFun Core Library Inline Functions
		 * @{
		 */

		/**
		 * Normalize a Vector3.
		 * Returns copy-elided normalized vector.
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
			return glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z)) *	  // position
				   glm::toMat4(glm::quat(rot.w, rot.x, rot.y, rot.z)) *					  // rotation
				   glm::translate(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z)); // matrix scale
		}

		/** @} */

		/**
		 * \defgroup FunTempls Core Library Function Templates
		 * @{
		 */

		// NOTE(lily): A lot of these are (ab)using
		// constexpr also being able to be used as a "try harder to inline/optimize me" signal to the compiler
		// (which actually works suprisingly well!).

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
		constexpr void ResizeMultiDimVec(std::vector<std::vector<T>>& vec, int dim1, int dim2) {
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
		constexpr bool DefaultMin(const T& LValue, const T& RValue) {
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
		constexpr bool DefaultMax(const T& LValue, const T& RValue) {
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
		constexpr typename RevIterContainer::value_type GetMaxElement(const RevIterContainer& container, MaxFun& Max = DefaultMax<typename RevIterContainer::value_type>) {
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
		constexpr typename RevIterContainer::value_type GetMinElement(const RevIterContainer& container, MinFun& Min = DefaultMin<typename RevIterContainer::value_type>) {
			typedef typename RevIterContainer::value_type T;

			auto it = std::min_element(container.begin(), container.end(), [&](const T& L, const T& R) {
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
		constexpr T Clamp(T Expression, const T Min, const T Max) {
			return (Expression > Max) ? Max : (Expression < Min) ? Min : Expression;
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
		constexpr typename RevIterContainer::iterator Where(RevIterContainer& vec, WhereFun evalulator) {
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
		constexpr std::size_t ArraySize(T (&)[N]) {
			return N;
		}

		/** @} */

		/**
		 * Namespace alias of the filesystem library, for shortening usage.
		 */
		namespace fs = std::filesystem;
	} // namespace core
} // namespace xb2at
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
#include <functional>
#include <algorithm>

namespace xb2at {
namespace core {

	/**
	 * \defgroup Types Types
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

	struct color {
		byte r;
		byte g;
		byte b;
		byte a;
	};
	
	/** @} */


	/**
	 * Normalize a Vector3.
	 *
	 * \param[in] vector Vector to normalize.
	 */
	inline void NormalizeVector3(vector3& vector)
	{
		double mag = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
		vector.x = vector.x / (float)mag;
		vector.y = vector.y / (float)mag;
		vector.z = vector.z / (float)mag;
	}

	/**
	 * \defgroup FunTempls Function Templates
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
	 * Progress reporting severity.
	 */
	enum ProgressType : int16 {
		Verbose,
		Info,
		Warning,
		Error
	};

	/**
	 * Namespace alias of the filesystem library.
	 */
	namespace fs = std::filesystem;
}
}
/**
 * \file 
 * Core library common include file.
 * This header is being phased out...
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



		template<class T>
		constexpr T MaxValue() {
			return std::numeric_limits<T>::max();
		}

		template<class T>
		constexpr T MinValue() {
			return std::numeric_limits<T>::min();
		}


		/** @} */

		/**
		 * \defgroup InlineFun Core Library Inline Functions
		 * @{
		 */





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
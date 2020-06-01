/**
 * \file 
 * Core library common include file
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
#include <functional> // std::function


namespace xb2at {
namespace core {

	typedef std::uint8_t byte;
	typedef std::int8_t sbyte;
	
	typedef std::int16_t int16;
	typedef std::uint16_t uint16;

	typedef std::int32_t int32;
	typedef std::uint32_t uint32;

	typedef std::int64_t int64;
	typedef std::uint64_t uint64;

	struct vector2 {
		float x;
		float y;
	};

	struct vector3 {
		float x;
		float y;
		float z;
	};

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

	inline void NormalizeVector3(vector3& vector)
	{
		double mag = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
		vector.x = vector.x / (float)mag;
		vector.y = vector.y / (float)mag;
		vector.z = vector.z / (float)mag;
	}
	
	template<class T>
	inline void ResizeMultiDimVec(std::vector<std::vector<T>>& vec, int dim1, int dim2) {
		vec.resize(dim1);

		for(std::vector<T>& v : vec)
			v.resize(dim2);
	}

	// Default max function
	template<typename T>
	inline bool DefaultMax(const T& LValue, const T& RValue) {
		return std::max(LValue, RValue) == RValue;
	}

	// Get max element of a vector
	template<class RevIterContainer, class WhereFun>
	inline typename RevIterContainer::value_type GetMaxElement(const RevIterContainer& vec) {
		typedef typename RevIterContainer::value_type T;
		auto it = std::max_element(vec.begin(), vec.end(), [&](const T& L, const T& R) {
			return std::max(L, R) == R;
		});

		return (*it);
	}

	// Default min function
	template<typename T>
	inline bool DefaultMin(const T& LValue, const T& RValue) {
		return std::min(LValue, RValue) == RValue;
	}

	// Get min element of a vector
	template<class RevIterContainer, class WhereFun>
	inline typename RevIterContainer::value_type GetMinElement(const RevIterContainer& vec) {
		typedef typename RevIterContainer::value_type T;

		auto it = std::min_element(vec.begin(), vec.end(), [&](const T& L, const T& R) {
			return std::min(L, R) == R;
		});

		return (*it);
	}

	template<typename T>
	inline T Clamp(T Expression, const T Min, const T Max) {
		return (Expression > Max) ? Max :
			(Expression < Min) ? Min : Expression;
	}

	/**
	 * Replacement for LINQ's .Where function.
	 */
	template<class RevIterContainer, class WhereFun>
	inline typename RevIterContainer::iterator Where(RevIterContainer& vec, WhereFun evalulator) {
		auto it = vec.begin();

		if(vec.size() == 0)
			return vec.end();

		for(int i = 0; i < vec.size(); ++i) {
			if(evalulator(*it))
				return it;
			it++;
		}

		return vec.end();
	}
	
	/**
	 * Progress reporting severity.
	 */
	enum ProgressType : int16 {
		Verbose,
		Info,
		Warning,
		Error
	};


	namespace fs = std::filesystem;
}
}
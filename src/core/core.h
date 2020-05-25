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
	template<typename T>
	inline T GetMaxElement(const std::vector<T>& vec) {
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
	template<typename T>
	inline T GetMinElement(const std::vector<T>& vec) {
		auto it = std::min_element(vec.begin(), vec.end(), [&](const T& L, const T& R) {
			return std::min(L, R) == R;
		});

		return (*it);
	}
	
	/**
	 * Progress reporting severity.
	 */
	enum ProgressType : int16 {
		Error,
		Warning,
		Info,
		Verbose
	};

	namespace fs = std::filesystem;
}
}
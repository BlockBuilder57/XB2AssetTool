//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_FOURCC_H
#define XB2AT_FOURCC_H

#include <cstdint>

namespace xb2at::core {

	/**
 	 * Make a FourCC value at compile time, with a given input string.
	 */
	constexpr std::uint32_t FourCCValue(const char fourCc[5]) {
		return (fourCc[0] << 24) | (fourCc[1] << 16) | (fourCc[2] << 8) | fourCc[3];
	}

} // namespace xb2at::core

#endif //XB2AT_FOURCC_H

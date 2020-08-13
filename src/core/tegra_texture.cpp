// This file is one big giant TODO.
// :P
#include <core.h>
#include "tegra_texture.h"

namespace xb2at {
namespace core {

	/**
	 * Info structure
	 */
	struct TexFormatInfo {
		uint32 bpp;
		
		uint32 blockwidth;

		uint32 blockheight;

		uint32 blockdepth;
	};


	/**
	 * Static dictionary of supported formats.
	 */
	static std::map<TextureFormat, TexFormatInfo> SupportedFormatTable = {
		// format, bpp, block width, block height, block depth
		{ TextureFormat::R8G8B8A8_UNORM, { 4, 1, 1, 1 } }
	};


namespace TegraInternal {

	int32 GetBpp(TegraTexture* Texture) {
		auto it = SupportedFormatTable.find(Texture->Format);

		if(it == SupportedFormatTable.end())
			return -1;

		auto& val = ((*it).second);

		return val.bpp;
	}

	int32 GetBlockWidth(TegraTexture* Texture) {
		auto it = SupportedFormatTable.find(Texture->Format);

		if(it == SupportedFormatTable.end())
			return -1;

		auto& val = ((*it).second);

		return val.blockwidth;
	}

	
	int32 GetBlockHeight(TegraTexture* Texture) {
		auto it = SupportedFormatTable.find(Texture->Format);

		if(it == SupportedFormatTable.end())
			return -1;

		auto& val = ((*it).second);

		return val.blockheight;
	}


	int32 GetBlockDepth(TegraTexture* Texture) {
		auto it = SupportedFormatTable.find(Texture->Format);

		if(it == SupportedFormatTable.end())
			return -1;

		auto& val = ((*it).second);

		return val.blockdepth;
	}

	// Ported from SwitchToolbox
	
	inline uint32 DivRoundup(uint32 n, uint32 d) { 
		return (n + d - 1) / d; 
	}

	inline uint32 roundup(uint32 x, uint32 y) { 
		return ((x - 1) | (y - 1)) + 1; 	
	}

	inline uint32 pow2_roundup(uint32 x) {
		x -= 1;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}
	

	// Really get block height.
	uint32 GetBlockHeightReal(uint32 height) {
		auto bh = pow2_roundup(height / 8);
		if (bh > 16)
			return 16;

		return bh;
	}


}

	bool TegraTexture::Deswizzle() {
		using namespace TegraInternal;

		int32 bpp = GetBpp(this);
		int32 blockWidth = GetBlockWidth(this);
		int32 blockHeight = GetBlockHeight(this);
		int32 blockDepth = GetBlockDepth(this);

		// format is probably unsupported.
		if(bpp == -1)
			return false;




		return true;
	}

}
}
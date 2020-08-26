#pragma once
#include <core.h>
#include <Logger.h>
#include <structs/mibl.h>

namespace xb2at {
	namespace core {

		/**
		 * Texture format in DirectX format
		 */
		enum class TextureFormat : uint32 {
			UNKNOWN,
			R32G32B32A32_TYPELESS,
			R32G32B32A32_FLOAT,
			R32G32B32A32_UINT,
			R32G32B32A32_SINT,
			R32G32B32_TYPELESS,
			R32G32B32_FLOAT,
			R32G32B32_UINT,
			R32G32B32_SINT,
			R16G16B16A16_TYPELESS,
			R16G16B16A16_FLOAT,
			R16G16B16A16_UNORM,
			R16G16B16A16_UINT,
			R16G16B16A16_SNORM,
			R16G16B16A16_SINT,
			R32G32_TYPELESS,
			R32G32_FLOAT,
			R32G32_UINT,
			R32G32_SINT,
			R32G8X24_TYPELESS,
			D32_FLOAT_S8X24_UINT,
			R32_FLOAT_X8X24_TYPELESS,
			X32_TYPELESS_G8X24_UINT,
			R10G10B10A2_TYPELESS,
			R10G10B10A2_UNORM,
			R10G10B10A2_UINT,
			R11G11B10_FLOAT,
			R8G8B8A8_TYPELESS,
			R8G8B8A8_UNORM,
			R8G8B8A8_UNORM_SRGB,
			R8G8B8A8_UINT,
			R8G8B8A8_SNORM,
			R8G8B8A8_SINT,
			R16G16_TYPELESS,
			R16G16_FLOAT,
			R16G16_UNORM,
			R16G16_UINT,
			R16G16_SNORM,
			R16G16_SINT,
			R32_TYPELESS,
			D32_FLOAT,
			R32_FLOAT,
			R32_UINT,
			R32_SINT,
			R24G8_TYPELESS,
			D24_UNORM_S8_UINT,
			R24_UNORM_X8_TYPELESS,
			X24_TYPELESS_G8_UINT,
			R8G8_TYPELESS,
			R8G8_UNORM,
			R8G8_UINT,
			R8G8_SNORM,
			R8G8_SINT,
			R16_TYPELESS,
			R16_FLOAT,
			D16_UNORM,
			R16_UNORM,
			R16_UINT,
			R16_SNORM,
			R16_SINT,
			R8_TYPELESS,
			R8_UNORM,
			R8_UINT,
			R8_SNORM,
			R8_SINT,
			A8_UNORM,
			R1_UNORM,
			R9G9B9E5_SHAREDEXP,
			R8G8_B8G8_UNORM,
			G8R8_G8B8_UNORM,
			BC1_TYPELESS,
			BC1_UNORM,
			BC1_UNORM_SRGB,
			BC2_TYPELESS,
			BC2_UNORM,
			BC2_UNORM_SRGB,
			BC3_TYPELESS,
			BC3_UNORM,
			BC3_UNORM_SRGB,
			BC4_TYPELESS,
			BC4_UNORM,
			BC4_SNORM,
			BC5_TYPELESS,
			BC5_UNORM,
			BC5_SNORM,
			B5G6R5_UNORM,
			B5G5R5A1_UNORM,
			B8G8R8A8_UNORM,
			B8G8R8X8_UNORM,
			R10G10B10_XR_BIAS_A2_UNORM,
			B8G8R8A8_TYPELESS,
			B8G8R8A8_UNORM_SRGB,
			B8G8R8X8_TYPELESS,
			B8G8R8X8_UNORM_SRGB,
			BC6H_TYPELESS,
			BC6H_UF16,
			BC6H_SF16,
			BC7_TYPELESS,
			BC7_UNORM,
			BC7_UNORM_SRGB,
			AYUV,
			Y410,
			Y416,
			NV12,
			P010,
			P016,
			DXGI_420_OPAQUE,
			YUY2,
			Y210,
			Y216,
			NV11,
			AI44,
			IA44,
			P8,
			A8P8,
			B4G4R4A4_UNORM,
			P208,
			V208,
			V408,
			SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
			SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
			FORCE_UINT
		};

		// NOTE: All of the things that are set
		// here are things that don't need to ever be changed.
		/**
		 * DDS header without DX SDK or Windows requirement
		 */
		struct DdsHeader {
			uint32 magic = 0x20534444;
			uint32 size = 124;
			uint32 flags = 0x1007;
			uint32 height;
			uint32 width;
			uint32 pitchOrLinearSize;
			uint32 depth = 1;
			uint32 mipCount = 1;
			uint32 reserved[11];

			/**
			 * Pixel format data structure
			 */
			struct PixelFormat {
				uint32 size = 32;
				uint32 flags = 0x4; // _FOURCC
				uint32 fourcc;
				uint32 RgbBitCount = 0;
				uint32 RbitMask = 0;
				uint32 GbitMask = 0;
				uint32 BbitMask = 0;
				uint32 AbitMask = 0;
			} pixFormat;

			uint32 caps = 0x1000;
			uint32 caps2 = 0;
			uint32 caps3 = 0;
			uint32 caps4 = 0;
			uint32 reserved2 = 0;

			/**
			 * DX10 DDS header
			 */
			struct Dx10Header {
				TextureFormat format;
				uint32 dimension = 3; // RESOURCE_DIMENSION_TEXTURE2D
				uint32 miscflag = 0;
				uint32 arraysize = 1;
				uint32 misc2 = 0;
			};
		};

		/**
		 * A Tegra X1 deswizzler for MIBL textures.
		 */
		struct MIBLDeswizzler {
			inline MIBLDeswizzler(mibl::texture& tex)
				: texture(tex) {
				// Convert from MIBL (nvn) format to DirectX format
				// since that's what we will use when exporting
				switch(tex.type) {
					case mibl::MiblTextureFormat::R8G8B8A8_UNORM:
						Format = TextureFormat::R8G8B8A8_UNORM;
						break;
					case mibl::MiblTextureFormat::BC1_UNORM:
						Format = TextureFormat::BC1_UNORM;
						break;
					case mibl::MiblTextureFormat::BC3_UNORM:
						Format = TextureFormat::BC3_UNORM;
						break;
					case mibl::MiblTextureFormat::BC4_UNORM:
						Format = TextureFormat::BC4_UNORM;
						break;
					case mibl::MiblTextureFormat::BC5_UNORM:
						Format = TextureFormat::BC5_UNORM;
						break;
					case mibl::MiblTextureFormat::BC7_UNORM:
						Format = TextureFormat::BC7_UNORM;
						break;

					default:
						logger.error("Unknown/Unhandled MIBL type ", (int)tex.type, "!");
						break;
				}
			}

			TextureFormat Format = TextureFormat::UNKNOWN;
			mibl::texture& texture;

			/**
			 * In-place deswizzle the MIBL.
			 */
			void Deswizzle();

			void Write(fs::path& path);

		   private:
			void SwizzleInternal(int bppPower, int swizzleSize = 4);

			Logger logger = Logger::CreateChannel("MIBLDeswizzler");
		};

	} // namespace core
} // namespace xb2at
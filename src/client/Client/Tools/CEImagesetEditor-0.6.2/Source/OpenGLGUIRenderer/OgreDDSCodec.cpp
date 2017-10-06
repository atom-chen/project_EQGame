/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/


#include "OgreDDSCodec.h"
#include "CEGUIExceptions.h"


namespace CEGUI
{

	//===================================================
	static unsigned int BitwiseIntRead(const void *src, int n) {
		switch(n) {
				case 1:
					return ((uint8*)src)[0];
				case 2:
					return ((uint16*)src)[0];
				case 3:
					return ((uint32)((uint8*)src)[0])|
						((uint32)((uint8*)src)[1]<<8)|
						((uint32)((uint8*)src)[2]<<16);
				case 4:
					return ((uint32*)src)[0];
		} 
		return 0; // ?
	}

	static float BitwiseFixedToFloat(unsigned value, unsigned int bits)
	{
		return (float)value/(float)((1<<bits)-1);
	}

	static inline void BitwiseIntWrite(void *dest, const int n, const unsigned int value)
	{
		switch(n) {
				case 1:
					((uint8*)dest)[0] = (uint8)value;
					break;
				case 2:
					((uint16*)dest)[0] = (uint16)value;
					break;
				case 3:
					((uint8*)dest)[2] = (uint8)((value >> 16) & 0xFF);
					((uint8*)dest)[1] = (uint8)((value >> 8) & 0xFF);
					((uint8*)dest)[0] = (uint8)(value & 0xFF);
					break;
				case 4:
					((uint32*)dest)[0] = (uint32)value;                
					break;                
		}        
	}

	static unsigned int BitwiseFloatToFixed(const float value, const unsigned int bits)
	{
		if(value <= 0.0f) return 0;
		else if (value >= 1.0f) return (1<<bits)-1;
		else return (unsigned int)(value * (1<<bits));     
	}

	static void unpackColour(float *r, float *g, float *b, float *a,
		PixelFormatForDDS pf,  const void* src)
	{
		assert( pf == PF_R5G6B5 );
		const int elemBytes = 2;

		// Shortcut for integer formats unpacking
		const unsigned int value = BitwiseIntRead(src, elemBytes);

		//注意这里的数值,写了立即数
		*r = BitwiseFixedToFloat((value & 0xF800)>>11, 5);
		*g = BitwiseFixedToFloat((value & 0x07E0)>>5, 6);
		*b = BitwiseFixedToFloat((value & 0x001F)>>0, 5);

		*a = 1.0f; // No alpha, default a component to full

	}

	static void unpackColour(ColourValue *colour, PixelFormatForDDS pf,  const void* src)
	{
		unpackColour(&colour->r, &colour->g, &colour->b, &colour->a, pf, src);
	}

	static void packColour(const float r, const float g, const float b, const float a, const PixelFormatForDDS pf,  void* dest)
	{
		unsigned int value;
		int elemBytes;

		if (pf == PF_BYTE_RGBA)
		{
			elemBytes = 4;
			value = ((BitwiseFloatToFixed(r, 8)<<0) & 0x000000FF) |
				((BitwiseFloatToFixed(g, 8)<<8) & 0x0000FF00) |
				((BitwiseFloatToFixed(b, 8)<<16) & 0x00FF0000) |
				((BitwiseFloatToFixed(a, 8)<<24) & 0xFF000000);
		}
		else //PF_BYTE_RGB
		{
			elemBytes = 3;
			value = ((BitwiseFloatToFixed(r, 8)<<0) & 0x0000FF) |
				((BitwiseFloatToFixed(g, 8)<<8) & 0x00FF00) |
				((BitwiseFloatToFixed(b, 8)<<16) & 0xFF0000) |
				((BitwiseFloatToFixed(a, 0)<<0) & 0);
		}

		// And write to memory
		BitwiseIntWrite(dest, elemBytes, value);
	}

	static void packColour(const ColourValue &colour, const PixelFormatForDDS pf,  void* dest)
	{
		packColour(colour.r, colour.g, colour.b, colour.a, pf, dest);
	}



	//===================================================
	const ColourValue ColourValue::ZERO = ColourValue(0.0,0.0,0.0,0.0);


	//===================================================
	class MemoryDataStream
	{
	public:
		size_t mSize;
		/// Pointer to the start of the data area
		uchar* mData;
		/// Pointer to the current position in the memory
		uchar* mPos;
		/// Pointer to the end of the memory
		uchar* mEnd;


	public:
		MemoryDataStream(void* pMem, size_t size)
		{
			mData = mPos = static_cast<uchar*>(pMem);
			mSize = size;
			mEnd = mData + mSize;
			assert(mEnd >= mPos);
		}

		size_t read(void* buf, size_t count)
		{
			size_t cnt = count;
			// Read over end of memory?
			if (mPos + cnt > mEnd)
				cnt = mEnd - mPos;
			if (cnt == 0)
				return 0;

			assert (cnt<=count);

			memcpy(buf, mPos, cnt);
			mPos += cnt;
			return cnt;
		}

		void skip(long count)
		{
			size_t newpos = (size_t)( ( mPos - mData ) + count );
			assert( mData + newpos <= mEnd );        

			mPos = mData + newpos;
		}
	};


	//==================================================
	class ImageData
	{
	public:
		ImageData():
		  height(0), width(0), depth(1), size(0),
			  num_mipmaps(0), flags(0), format(PF_UNKNOWN)
		  {
		  }
		  size_t height;
		  size_t width;
		  size_t depth;
		  size_t size;

		  ushort num_mipmaps;
		  uint flags;

		  PixelFormatForDDS format;
	};


	//===================================================
#define Real float
	// Internal DDS structure definitions
#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
	
#pragma pack (push, 1)


	// Nested structure
	struct DDSPixelFormat
	{
		uint32 size;
		uint32 flags;
		uint32 fourCC;
		uint32 rgbBits;
		uint32 redMask;
		uint32 greenMask;
		uint32 blueMask;
		uint32 alphaMask;
	};
	
	// Nested structure
	struct DDSCaps
	{
		uint32 caps1;
		uint32 caps2;
		uint32 reserved[2];
	};
	// Main header, note preceded by 'DDS '
	struct DDSHeader
	{
		uint32 size;		
		uint32 flags;
		uint32 height;
		uint32 width;
		uint32 sizeOrPitch;
		uint32 depth;
		uint32 mipMapCount;
		uint32 reserved1[11];
		DDSPixelFormat pixelFormat;
		DDSCaps caps;
		uint32 reserved2;
	};

	// An 8-byte DXT colour block, represents a 4x4 texel area. Used by all DXT formats
	struct DXTColourBlock
	{
		// 2 colour ranges
		uint16 colour_0;
		uint16 colour_1;
		// 16 2-bit indexes, each byte here is one row
		uint8 indexRow[4];
	};
	// An 8-byte DXT explicit alpha block, represents a 4x4 texel area. Used by DXT2/3
	struct DXTExplicitAlphaBlock
	{
		// 16 4-bit values, each 16-bit value is one row
		uint16 alphaRow[4];
	};
	// An 8-byte DXT interpolated alpha block, represents a 4x4 texel area. Used by DXT4/5
	struct DXTInterpolatedAlphaBlock
	{
		// 2 alpha ranges
		uint8 alpha_0;
		uint8 alpha_1;
		// 16 3-bit indexes. Unfortunately 3 bits doesn't map too well to row bytes
		// so just stored raw
		uint8 indexes[6];
	};
	

#pragma pack (pop)


	const uint32 DDS_PIXELFORMAT_SIZE = 8 * sizeof(uint32);
	const uint32 DDS_CAPS_SIZE = 4 * sizeof(uint32);
	const uint32 DDS_HEADER_SIZE = 19 * sizeof(uint32) + DDS_PIXELFORMAT_SIZE + DDS_CAPS_SIZE;

	const uint32 DDSD_CAPS = 0x00000001;
	const uint32 DDSD_HEIGHT = 0x00000002;
	const uint32 DDSD_WIDTH = 0x00000004;
	const uint32 DDSD_PITCH = 0x00000008;
	const uint32 DDSD_PIXELFORMAT = 0x00001000;
	const uint32 DDSD_MIPMAPCOUNT = 0x00020000;
	const uint32 DDSD_LINEARSIZE = 0x00080000;
	const uint32 DDSD_DEPTH = 0x00800000;
	const uint32 DDPF_ALPHAPIXELS = 0x00000001;
	const uint32 DDPF_FOURCC = 0x00000004;
	const uint32 DDPF_RGB = 0x00000040;
	const uint32 DDSCAPS_COMPLEX = 0x00000008;
	const uint32 DDSCAPS_TEXTURE = 0x00001000;
	const uint32 DDSCAPS_MIPMAP = 0x00400000;
	const uint32 DDSCAPS2_CUBEMAP = 0x00000200;
	const uint32 DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400;
	const uint32 DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800;
	const uint32 DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000;
	const uint32 DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000;
	const uint32 DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000;
	const uint32 DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000;
	const uint32 DDSCAPS2_VOLUME = 0x00200000;

	// Special FourCC codes
	const uint32 D3DFMT_R16F			= 111;
	const uint32 D3DFMT_G16R16F			= 112;
	const uint32 D3DFMT_A16B16G16R16F	= 113;
	const uint32 D3DFMT_R32F            = 114;
	const uint32 D3DFMT_G32R32F         = 115;
	const uint32 D3DFMT_A32B32G32R32F   = 116;


	//---------------------------------------------------------------------
    DDSCodec::DDSCodec()
    { 
    }
	//---------------------------------------------------------------------
	PixelFormatForDDS DDSCodec::convertFourCCFormat(uint32 fourcc) const
	{
		// convert dxt pixel format
		switch(fourcc)
		{
		case FOURCC('D','X','T','1'):
			return PF_DXT1;
		case FOURCC('D','X','T','2'):
			return PF_DXT2;
		case FOURCC('D','X','T','3'):
			return PF_DXT3;
		case FOURCC('D','X','T','4'):
			return PF_DXT4;
		case FOURCC('D','X','T','5'):
			return PF_DXT5;
		//case D3DFMT_R16F:
		//	return PF_FLOAT16_R;
		//case D3DFMT_G16R16F:
		//	return PF_FLOAT16_GR;
		//case D3DFMT_A16B16G16R16F:
		//	return PF_FLOAT16_RGBA;
		//case D3DFMT_R32F:
		//	return PF_FLOAT32_R;
		//case D3DFMT_G32R32F:
		//	return PF_FLOAT32_GR;
		//case D3DFMT_A32B32G32R32F:
		//	return PF_FLOAT32_RGBA;
		// We could support 3Dc here, but only ATI cards support it, not nVidia
		default:
			throw RendererException("Unsupported FourCC format found in DDS file");
		};

	}
	//---------------------------------------------------------------------
	//PixelFormatForDDS DDSCodec::convertPixelFormat(uint32 rgbBits, uint32 rMask, 
	//	uint32 gMask, uint32 bMask, uint32 aMask) const
	//{
	//	// General search through pixel formats
	//	for (int i = PF_UNKNOWN + 1; i < PF_COUNT; ++i)
	//	{
	//		PixelFormatForDDS pf = static_cast<PixelFormatForDDS>(i);
	//		if (PixelUtil::getNumElemBits(pf) == rgbBits)
	//		{
	//			uint32 testMasks[4];
	//			PixelUtil::getBitMasks(pf, testMasks);
	//			int testBits[4];
	//			PixelUtil::getBitDepths(pf, testBits);
	//			if (testMasks[0] == rMask && testMasks[1] == gMask &&
	//				testMasks[2] == bMask && 
	//				// for alpha, deal with 'X8' formats by checking bit counts
	//				(testMasks[3] == aMask || (aMask == 0 && testBits[3] == 0)))
	//			{
	//				return pf;
	//			}
	//		}

	//	}

	//	throw RendererException("Cannot determine pixel format");

	//}
	//---------------------------------------------------------------------
	void DDSCodec::unpackDXTColour(PixelFormatForDDS pf, const DXTColourBlock& block, 
		ColourValue* pCol) const
	{
		// Note - we assume all values have already been endian swapped

		// Colour lookup table
		ColourValue derivedColours[4];

		if (pf == PF_DXT1 && block.colour_0 <= block.colour_1)
		{
			// 1-bit alpha
			unpackColour(&(derivedColours[0]), PF_R5G6B5, &(block.colour_0));
			unpackColour(&(derivedColours[1]), PF_R5G6B5, &(block.colour_1));
			// one intermediate colour, half way between the other two
			derivedColours[2] = (derivedColours[0] + derivedColours[1]) / 2;
			// transparent colour
			derivedColours[3] = ColourValue::ZERO;
		}
		else
		{
			unpackColour(&(derivedColours[0]), PF_R5G6B5, &(block.colour_0));
			unpackColour(&(derivedColours[1]), PF_R5G6B5, &(block.colour_1));
			// first interpolated colour, 1/3 of the way along
			derivedColours[2] = (2 * derivedColours[0] + derivedColours[1]) / 3;
			// second interpolated colour, 2/3 of the way along
			derivedColours[3] = (derivedColours[0] + 2 * derivedColours[1]) / 3;
		}

		// Process 4x4 block of texels
		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t x = 0; x < 4; ++x)
			{
				// LSB come first
				uint8 colIdx = static_cast<uint8>(block.indexRow[row] >> (x * 2) & 0x3);
				if (pf == PF_DXT1)
				{
					// Overwrite entire colour
					pCol[(row * 4) + x] = derivedColours[colIdx];
				}
				else
				{
					// alpha has already been read (alpha precedes colour)
					ColourValue& col = pCol[(row * 4) + x];
					col.r = derivedColours[colIdx].r;
					col.g = derivedColours[colIdx].g;
					col.b = derivedColours[colIdx].b;
				}
			}

		}


	}
	//---------------------------------------------------------------------
	void DDSCodec::unpackDXTAlpha(
		const DXTExplicitAlphaBlock& block, ColourValue* pCol) const
	{
		// Note - we assume all values have already been endian swapped
		
		// This is an explicit alpha block, 4 bits per pixel, LSB first
		for (size_t row = 0; row < 4; ++row)
		{
			for (size_t x = 0; x < 4; ++x)
			{
				// Shift and mask off to 4 bits
				uint8 val = static_cast<uint8>(block.alphaRow[row] >> (x * 4) & 0xF);
				// Convert to [0,1]
				pCol->a = (Real)val / (Real)0xF;
				pCol++;
				
			}
			
		}

	}
	//---------------------------------------------------------------------
	void DDSCodec::unpackDXTAlpha(
		const DXTInterpolatedAlphaBlock& block, ColourValue* pCol) const
	{
		// 8 derived alpha values to be indexed
		Real derivedAlphas[8];

		// Explicit extremes
		derivedAlphas[0] = block.alpha_0 / (Real)0xFF;
		derivedAlphas[1] = block.alpha_1 / (Real)0xFF;


		if (block.alpha_0 <= block.alpha_1)
		{
			// 4 interpolated alphas, plus zero and one			
			// full range including extremes at [0] and [5]
			// we want to fill in [1] through [4] at weights ranging
			// from 1/5 to 4/5
			Real denom = 1.0f / 5.0f;
			for (size_t i = 0; i < 4; ++i) 
			{
				Real factor0 = (4 - i) * denom;
				Real factor1 = (i + 1) * denom;
				derivedAlphas[i + 2] = 
					(factor0 * block.alpha_0) + (factor1 * block.alpha_1);
			}
			derivedAlphas[6] = 0.0f;
			derivedAlphas[7] = 1.0f;

		}
		else
		{
			// 6 interpolated alphas
			// full range including extremes at [0] and [7]
			// we want to fill in [1] through [6] at weights ranging
			// from 1/7 to 6/7
			Real denom = 1.0f / 7.0f;
			for (size_t i = 0; i < 6; ++i) 
			{
				Real factor0 = (6 - i) * denom;
				Real factor1 = (i + 1) * denom;
				derivedAlphas[i + 2] = 
					(factor0 * block.alpha_0) + (factor1 * block.alpha_1);
			}

		}

		// Ok, now we've built the reference values, process the indexes
		for (size_t i = 0; i < 16; ++i)
		{
			size_t baseByte = (i * 3) / 8;
			size_t baseBit = (i * 3) % 8;
			uint8 bits = static_cast<uint8>(block.indexes[baseByte] >> baseBit & 0x7);
			// do we need to stitch in next byte too?
			if (baseBit > 5)
			{
				uint8 extraBits = static_cast<uint8>(
					(block.indexes[baseByte+1] << (8 - baseBit)) & 0xFF);
				bits |= extraBits & 0x7;
			}
			pCol[i].a = derivedAlphas[bits];
			pCol[i].a = 1.0f;		//透明出错了, 暂时不要透明. 这是临时解决方案 --Y3

		}

	}
    //---------------------------------------------------------------------
	Texture* DDSCodec::decode(RawDataContainer& input, Texture* result) const
    {
		MemoryDataStream ds(input.getDataPtr(), input.getSize());
		MemoryDataStream* stream = &ds;

		// Read 4 character code
		uint32 fileType;
		stream->read(&fileType, sizeof(uint32));
		flipEndian(&fileType, sizeof(uint32), 1);
		
		if (FOURCC('D', 'D', 'S', ' ') != fileType)
		{
			throw RendererException("This is not a DDS file!");
		}
		
		// Read header in full
		DDSHeader header;
		stream->read(&header, sizeof(DDSHeader));

		// Endian flip if required, all 32-bit values
		flipEndian(&header, 4, sizeof(DDSHeader) / 4);

		// Check some sizes
		if (header.size != DDS_HEADER_SIZE)
		{
			throw RendererException("DDS header size mismatch!");
		}
		if (header.pixelFormat.size != DDS_PIXELFORMAT_SIZE)
		{
			throw RendererException("DDS header size mismatch!");

		}

		ImageData imgD;
		ImageData* imgData = &imgD;
		//MemoryDataStreamPtr output;

		imgData->depth = 1; // (deal with volume later)
		imgData->width = header.width;
		imgData->height = header.height;
		size_t numFaces = 1; // assume one face until we know otherwise

		if (header.caps.caps1 & DDSCAPS_MIPMAP)
		{
	        imgData->num_mipmaps = header.mipMapCount - 1;
		}
		else
		{
			imgData->num_mipmaps = 0;
		}
		imgData->flags = 0;

		bool decompressDXT = false;
		// Figure out basic image type
		if (header.caps.caps2 & DDSCAPS2_CUBEMAP)
		{
			//imgData->flags |= IF_CUBEMAP;		//没用
			numFaces = 6;
		}
		else if (header.caps.caps2 & DDSCAPS2_VOLUME)
		{
			//imgData->flags |= IF_3D_TEXTURE;	//没用
			imgData->depth = header.depth;
		}
		// Pixel format
		PixelFormatForDDS sourceFormat = PF_UNKNOWN;

		if (header.pixelFormat.flags & DDPF_FOURCC)
		{
			sourceFormat = convertFourCCFormat(header.pixelFormat.fourCC);
		}
		else
		{
			throw RendererException("convertPixelFormat is no used!!");
			//sourceFormat = convertPixelFormat(header.pixelFormat.rgbBits, 
			//	header.pixelFormat.redMask, header.pixelFormat.greenMask, 
			//	header.pixelFormat.blueMask, 
			//	header.pixelFormat.flags & DDPF_ALPHAPIXELS ? 
			//	header.pixelFormat.alphaMask : 0);
		}

		if (/*PixelUtil::isCompressed(sourceFormat)*/true)	//一定是压缩的dds就是DXT1~5
		{
			//if (!Root::getSingleton().getRenderSystem()->getCapabilities()
			//	->hasCapability(RSC_TEXTURE_COMPRESSION_DXT))
			if(true)		//我们需要解压这个dds
			{
				// We'll need to decompress
				decompressDXT = true;
				// Convert format
				switch (sourceFormat)
				{
				case PF_DXT1:
					// source can be either 565 or 5551 depending on whether alpha present
					// unfortunately you have to read a block to figure out which
					// Note that we upgrade to 32-bit pixel formats here, even 
					// though the source is 16-bit; this is because the interpolated
					// values will benefit from the 32-bit results, and the source
					// from which the 16-bit samples are calculated may have been
					// 32-bit so can benefit from this.
					DXTColourBlock block;
					stream->read(&block, sizeof(DXTColourBlock));
					flipEndian(&(block.colour_0), sizeof(uint16), 1);
					flipEndian(&(block.colour_1), sizeof(uint16), 1);
					// skip back since we'll need to read this again
					stream->skip(0 - sizeof(DXTColourBlock));
					// colour_0 <= colour_1 means transparency in DXT1
					if (block.colour_0 <= block.colour_1)
					{
						imgData->format = PF_BYTE_RGBA;
					}
					else
					{
						imgData->format = PF_BYTE_RGB;
					}
					break;
				case PF_DXT2:
				case PF_DXT3:
				case PF_DXT4:
				case PF_DXT5:
					// full alpha present, formats vary only in encoding 
					imgData->format = PF_BYTE_RGBA;
					break;
                default:
                    // all other cases need no special format handling
                    break;
				}
			}
			//else
			//{
			//	// Use original format
			//	imgData->format = sourceFormat;
			//	// Keep DXT data compressed
			//	imgData->flags |= IF_COMPRESSED;
			//}
		}
		//else // not compressed
		//{
		//	// Don't test against DDPF_RGB since greyscale DDS doesn't set this
		//	// just derive any other kind of format
		//	imgData->format = sourceFormat;
		//}

		// Calculate total size from number of mipmaps, faces and size
		//imgData->size = Image::calculateSize(imgData->num_mipmaps, numFaces, 
		//	imgData->width, imgData->height, imgData->depth, imgData->format);
		
		//我们只取图像第一层
		size_t NumElemBytes;
		if (imgData->format == PF_BYTE_RGBA)
		{
			NumElemBytes = 4;
		}
		else	//PF_BYTE_RGB
		{
			NumElemBytes = 3;
		}
		imgData->size = imgData->width * imgData->height * NumElemBytes;

		// Bind output buffer
		//output.bind(OGRE_NEW MemoryDataStream(imgData->size));
		void* output = new char[imgData->size];

		
		// Now deal with the data
		void* destPtr = output;

		// all mips for a face, then each face
		for(size_t i = 0; i < /*numFaces*/1; ++i)	//1面
		{   
			size_t width = imgData->width;
			size_t height = imgData->height;
			size_t depth = imgData->depth;
			depth = 1;//取一个

			for(size_t mip = 0; mip <= /*imgData->num_mipmaps*/0; ++mip) //1层
			{
				size_t dstPitch = width * NumElemBytes;

				if (/*PixelUtil::isCompressed(sourceFormat)*/true)	//必须是true
				{
					// Compressed data
					if (decompressDXT)	//一定是true
					{
						DXTColourBlock col;
						DXTInterpolatedAlphaBlock iAlpha;
						DXTExplicitAlphaBlock eAlpha;
						// 4x4 block of decompressed colour
						ColourValue tempColours[16];
						size_t destBpp = NumElemBytes;
						size_t sx = std::min(width, (size_t)4);
						size_t sy = std::min(height, (size_t)4);
						size_t destPitchMinus4 = dstPitch - destBpp * sx;
						// slices are done individually
						for(size_t z = 0; z < depth; ++z)
						{
							// 4x4 blocks in x/y
							for (size_t y = 0; y < height; y += 4)
							{
								for (size_t x = 0; x < width; x += 4)
								{
									if (sourceFormat == PF_DXT2 || 
										sourceFormat == PF_DXT3)
									{
										// explicit alpha
										stream->read(&eAlpha, sizeof(DXTExplicitAlphaBlock));
										flipEndian(eAlpha.alphaRow, sizeof(uint16), 4);
										unpackDXTAlpha(eAlpha, tempColours) ;
									}
									else if (sourceFormat == PF_DXT4 || 
										sourceFormat == PF_DXT5)
									{
										// interpolated alpha
										stream->read(&iAlpha, sizeof(DXTInterpolatedAlphaBlock));
										flipEndian(&(iAlpha.alpha_0), sizeof(uint16), 1);
										flipEndian(&(iAlpha.alpha_1), sizeof(uint16), 1);
										unpackDXTAlpha(iAlpha, tempColours) ;
									}
									// always read colour
									stream->read(&col, sizeof(DXTColourBlock));
									flipEndian(&(col.colour_0), sizeof(uint16), 1);
									flipEndian(&(col.colour_1), sizeof(uint16), 1);
									unpackDXTColour(sourceFormat, col, tempColours);

									// write 4x4 block to uncompressed version
									for (size_t by = 0; by < sy; ++by)
									{
										for (size_t bx = 0; bx < sx; ++bx)
										{
											packColour(tempColours[by*4+bx],
												imgData->format, destPtr);
											destPtr = static_cast<void*>(
												static_cast<uchar*>(destPtr) + destBpp);
										}
										// advance to next row
										destPtr = static_cast<void*>(
											static_cast<uchar*>(destPtr) + destPitchMinus4);
									}
									// next block. Our dest pointer is 4 lines down
									// from where it started
									if (x + 4 >= width)
									{
										// Jump back to the start of the line
										destPtr = static_cast<void*>(
											static_cast<uchar*>(destPtr) - destPitchMinus4);
									}
									else
									{
										// Jump back up 4 rows and 4 pixels to the
										// right to be at the next block to the right
										destPtr = static_cast<void*>(
											static_cast<uchar*>(destPtr) - dstPitch * sy + destBpp * sx);

									}

								}

							}
						}

					}
					//else
					//{
					//	// load directly
					//	// DDS format lies! sizeOrPitch is not always set for DXT!!
					//	size_t dxtSize = PixelUtil::getMemorySize(width, height, depth, imgData->format);
					//	stream->read(destPtr, dxtSize);
					//	destPtr = static_cast<void*>(static_cast<uchar*>(destPtr) + dxtSize);
					//}

				}
			//	else
			//	{
			//		// Final data - trim incoming pitch
			//		size_t srcPitch;
			//		if (header.flags & DDSD_PITCH)
			//		{
			//			srcPitch = header.sizeOrPitch / 
			//				std::max((size_t)1, mip * 2);
			//		}
			//		else
			//		{
			//			// assume same as final pitch
			//			srcPitch = dstPitch;
			//		}
			//		assert (dstPitch <= srcPitch);
			//		long srcAdvance = static_cast<long>(srcPitch) - static_cast<long>(dstPitch);

			//		for (size_t z = 0; z < imgData->depth; ++z)
			//		{
			//			for (size_t y = 0; y < imgData->height; ++y)
			//			{
			//				stream->read(destPtr, dstPitch);
			//				if (srcAdvance > 0)
			//					stream->skip(srcAdvance);

			//				destPtr = static_cast<void*>(static_cast<uchar*>(destPtr) + dstPitch);
			//			}
			//		}

			//	}

			//	
			//	/// Next mip
			//	if(width!=1) width /= 2;
			//	if(height!=1) height /= 2;
			//	if(depth!=1) depth /= 2;
			}

		}

		//DecodeResult ret;
		//ret.first = output;
		//ret.second = CodecDataPtr(imgData);
		//return ret;
		
		Texture::PixelFormat cefmt = Texture::PF_RGB;
		if (imgData->format == PF_BYTE_RGBA)
		{
			cefmt = Texture::PF_RGBA;
		}

		result->loadFromMemory(output, imgData->width, imgData->height, cefmt);

		delete[] output;

		return result;

    }
    //---------------------------------------------------------------------    
    void DDSCodec::flipEndian(void * pData, size_t size, size_t count) const
    {
    }
    //---------------------------------------------------------------------    
    void DDSCodec::flipEndian(void * pData, size_t size) const
    {
    }
	
}
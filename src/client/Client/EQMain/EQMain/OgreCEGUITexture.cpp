/************************************************************************
	filename: 	OgreCEGUITexture.cpp
	created:	11/5/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Texture using Ogre engine
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "EQMainPCH.h"
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIExceptions.h>
#include "OgreCEGUITexture.h"
#include "OgreCEGUIRenderer.h"

#include <Ogre/OgreTextureManager.h>

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static data definition / initialisation
*************************************************************************/
uint32 OgreCEGUITexture::d_texturenumber		= 0;


/*************************************************************************
	Constructor
*************************************************************************/
OgreCEGUITexture::OgreCEGUITexture(Renderer* owner) :
	Texture(owner)
{
	d_ogre_texture.setNull();
	d_isLinked = false;
	d_delay = false;
	d_lastVisitTex = 0;
}


/*************************************************************************
	Destructor
*************************************************************************/
OgreCEGUITexture::~OgreCEGUITexture(void)
{
	freeOgreTexture();
}


/*************************************************************************
	constLoad
*************************************************************************/
void OgreCEGUITexture::constLoad() const
{
	if (d_delay)
	{
		assert(d_ogre_texture.isNull());
		assert(d_isLinked == false);
		Ogre::TexturePtr ogreTexture = (Ogre::TexturePtr)Ogre::TextureManager::getSingleton().getByName(d_delayLoadFile.c_str());
		if (!ogreTexture.isNull())
		{
			std::string err = d_delayLoadFile + " file exist!";
			throw RendererException((utf8*)err.c_str());
		}

		d_ogre_texture = Ogre::TextureManager::getSingleton().load(d_delayLoadFile, d_delayGroup, Ogre::TEX_TYPE_2D, 0, 1.0f);

		d_width		= d_ogre_texture->getWidth();
		d_height	= d_ogre_texture->getHeight();
		d_delay = false;
	}
}


/*************************************************************************
	Loads the specified image file into the texture.  The texture is
	resized as required to hold the image.	
*************************************************************************/
void OgreCEGUITexture::loadFromFile(const String& filename, const String& resourceGroup)
{
	using namespace Ogre;

	// unload old ogre texture
	freeOgreTexture();

	// create / load a new ogre texture from the specified image
	try
	{
		TextureManager& textureManager = TextureManager::getSingleton();

		// see if texture already exists
		Ogre::TexturePtr ogreTexture = (Ogre::TexturePtr)textureManager.getByName(filename.c_str());

		if (!ogreTexture.isNull())
		{
			// texture already exists, so create a 'linked' texture (ensures texture is not destroyed twice)
			d_ogre_texture = ogreTexture;
			d_isLinked = true;
		}
		// texture does not already exist, so load it in
		else
		{
            String orpGroup;
            if (resourceGroup.empty())
            {
                const String& defGrp = CEGUI::System::getSingleton().getResourceProvider()->getDefaultResourceGroup();
                orpGroup = defGrp.empty() ? Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME.c_str() : defGrp;
            }
            else
            {
                orpGroup = resourceGroup;
            }

			//d_ogre_texture = TextureManager::getSingleton().load(filename.c_str(), orpGroup.c_str(), TEX_TYPE_2D, 0, 1.0f);
			d_delay = true;
			d_delayLoadFile = filename.c_str();
			d_delayGroup = orpGroup.c_str();

			d_isLinked = false;
		}

	}
	catch(Ogre::Exception e)
	{
		throw RendererException((utf8*)"Failed to create Texture object from file '" + filename + "'. Additional Information:\n" + e.getFullDescription().c_str());
	}

	// if we got a pointer cache some details
	d_width		= 0;
	d_height	= 0;
	if (!d_ogre_texture.isNull() )
	{
		d_width		= d_ogre_texture->getWidth();
		d_height	= d_ogre_texture->getHeight();
	}
	// no texture from image so throw.
	else if ( !d_delay )
	{
		throw RendererException((utf8*)"Failed to create Texture object from file '" + filename + "'.  Ogre returned a NULL pointer.");
	}

}


/*************************************************************************
	Loads (copies) an image in memory into the texture.  The texture is
	resized as required to hold the image.	
*************************************************************************/

void _byteSwap(unsigned char* b, int n)
{
    register int i = 0;
    register int j = n-1;
    while (i<j)
    {
        std::swap(b[i], b[j]);
        i++, j--;
    }
}
#define byteSwap(x) _byteSwap((unsigned char*) &x,sizeof(x))

void OgreCEGUITexture::loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat)
{
	using namespace Ogre;

	// get rid of old texture
	freeOgreTexture();

	// wrap input buffer with an Ogre DataChunk
    uint32 bytesize = ((buffWidth * sizeof(uint32)) * buffHeight);

#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
    uint32* swappedBuffer = new uint32[bytesize/4];
    memcpy(swappedBuffer, buffPtr, bytesize);

    for (int i=0; i < bytesize/4; i++)
        byteSwap(swappedBuffer[i]);

#pragma push_macro("new")
#undef new
    DataStreamPtr odc(OGRE_NEW MemoryDataStream(static_cast<void*>(swappedBuffer), bytesize, false));
#pragma pop_macro("new")
#else
#pragma push_macro("new")
#undef new
	DataStreamPtr odc(OGRE_NEW MemoryDataStream(const_cast<void*>(buffPtr), bytesize, false));
#pragma pop_macro("new")
#endif

	// get pixel type for the target texture - the elements here might look wrong, but is just
	// differences in definition (at the core level, between GL and D3D).
	Ogre::PixelFormat targetFmt =
		(pixelFormat == PF_RGBA) ? Ogre::PF_A8R8G8B8 : Ogre::PF_R8G8B8;

	// try to create a Ogre::Texture from the input data
	d_ogre_texture = TextureManager::getSingleton().loadRawData(getUniqueName(), "General", odc, buffWidth, buffHeight, targetFmt , TEX_TYPE_2D, 0, 1.0f);
	d_isLinked = false;

	// if we got a pointer cache some details
	if (!d_ogre_texture.isNull())
	{
		d_width		= d_ogre_texture->getWidth();
		d_height	= d_ogre_texture->getHeight();
	}
	// no texture from memory so throw.
	else
	{
		throw RendererException((utf8*)"Failed to create Texture object from memory:  Ogre returned a NULL Ogre::Texture pointer.");
	}

}


/*************************************************************************
	set the size of the internal Ogre texture.  Previous Ogre texture
	is lost.	
*************************************************************************/
void OgreCEGUITexture::setOgreTextureSize(uint size)
{
	using namespace Ogre;

	// unload any current Ogre::Texture
	freeOgreTexture();

	// Try to create an empty texture of the given size
	d_ogre_texture = TextureManager::getSingleton().createManual(getUniqueName(), "General", TEX_TYPE_2D, size, size, 0, PF_A8R8G8B8, TU_DEFAULT);

	// if we got a pointer cache some details
	if (!d_ogre_texture.isNull())
	{
		d_width		= d_ogre_texture->getWidth();
		d_height	= d_ogre_texture->getHeight();
	}
	// no texture so throw.
	else
	{
		throw RendererException((utf8*)"Failed to create texture of specified size: Ogre::Texture creation failed.");
	}

}


/*************************************************************************
	safely free Ogre::Texture texture (can be called multiple times with
	no ill effect)
*************************************************************************/
void OgreCEGUITexture::freeOgreTexture(void)
{
	if ((!d_ogre_texture.isNull()) && !d_isLinked)
	{
		Ogre::TextureManager::getSingleton().remove(d_ogre_texture->getHandle());
	}
	d_ogre_texture.setNull();
	d_isLinked = false;
	d_delay = false;
	d_delayLoadFile.clear();
	d_delayGroup.clear();
}

/*************************************************************************
getOgreTexture
*************************************************************************/
Ogre::TexturePtr OgreCEGUITexture::getOgreTexture(void)
{
	constLoad();
	d_lastVisitTex = clock();
	return d_ogre_texture;
}

/*************************************************************************
canRecycle
*************************************************************************/
bool OgreCEGUITexture::canRecycle()
{
	if ( !d_delayLoadFile.empty()			// 有延后读取的文件, 说明资源从硬盘读取, 可以回收
		&& !d_delay )						// 延后读取过了, 即为false, 则可以回收. (初始状态也是false, 不过这时d_delayLoadFile应该为null)
	{
		assert(!d_ogre_texture.isNull());	// 这是他应该有数据
		return true;
	}

	return false;
}

/*************************************************************************
recycleTexture
*************************************************************************/
bool OgreCEGUITexture::recycleTexture(size_t& size)
{
	if ( canRecycle() )
	{
		assert(d_isLinked == false);		//不应该是有链接的

		//不正确的引用次数. 只能由自己引用一次和d_quadlist引用一次. 
		//+2这种判定不是很准确,因为d_quadlist未必会引用, 如果不幸是这种原因导致的, 那么可能造成内存泄露. 错了, 可能会多次引用, 这个办法无效, 应该在之前清空d_quadlist
		if (d_ogre_texture.useCount() != Ogre::ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS + 1
			/*&& d_ogre_texture.useCount() != Ogre::ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS + 2*/)
		{
			throw RendererException((utf8*)((d_ogre_texture->getName() + " useCount err in -OgreCEGUITexture::recycleTexture-").c_str()));
		}
		size = d_ogre_texture->getSize();
		//回收资源
		Ogre::TextureManager::getSingleton().remove(d_ogre_texture->getHandle());

		d_ogre_texture.setNull();
		d_delay = true;
		return true;
	}


	return false;
}

size_t OgreCEGUITexture::getTextureSize()
{
	if (d_ogre_texture.isNull())
		return 0;
	else
		return d_ogre_texture->getSize();
}

/*************************************************************************
	return a Ogre::string that contains a unique name.	
*************************************************************************/
Ogre::String OgreCEGUITexture::getUniqueName(void)
{
	Ogre::String str;

#ifdef CEGUI_USEOLDOGRESTRING
	str << "_cegui_ogre_" << d_texturenumber;
#else
	Ogre::StringUtil::StrStreamType strstream;
	strstream << "_cegui_ogre_" << d_texturenumber;
	str = strstream.str();
#endif

	++d_texturenumber;

	return str;
}


/*************************************************************************
	Set the internal Ogre::Texture object.
*************************************************************************/
void OgreCEGUITexture::setOgreTexture(Ogre::TexturePtr& texture)
{
	freeOgreTexture();

	d_ogre_texture = texture;
	d_width	 = d_ogre_texture->getWidth();
	d_height = d_ogre_texture->getHeight();
	d_isLinked = true;
}




/*************************************************************************
Set the internal Ogre::Texture object.
*************************************************************************/
void OgreCEGUITexture::updateRect( uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat )
{
	using namespace Ogre;

	constLoad();

	Ogre::Image::Box box(x, y, x + buffWidth, y + buffHeight);

	HardwarePixelBufferSharedPtr pixelBuffer = d_ogre_texture->getBuffer();
	pixelBuffer->lock(box, HardwareBuffer::HBL_NORMAL);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();

	ulong* dst = static_cast<ulong*>(pixelBox.data);
	ulong* src = (ulong*)buffPtr;


	// RGBA
	if (pixelFormat == PF_RGBA)
	{
		for (uint i = 0; i < buffHeight; ++i)
		{
			for (uint j = 0; j < buffWidth; ++j)
			{
				dst[j] = src[j];
			}

			dst += pixelBox.rowPitch;
			src += buffWidth;
		}
	}
	// RGB
	else
	{
		throw RendererException((utf8*)"no test here! so excepton --Y3!");

		for (uint i = 0; i < buffHeight; ++i)
		{
			for (uint j = 0; j < buffWidth; ++j)
			{
				dst[j] = src[j];
			}

			dst += pixelBox.rowPitch;
			src += buffWidth;
		}
	}

	pixelBuffer->unlock();

}



} // End of  CEGUI namespace section
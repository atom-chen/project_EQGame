/************************************************************************
	filename: 	OgreCEGUITexture.h
	created:	11/5/2004
	author:		Paul D Turner
	
	purpose:	Interface to Texture implemented via Ogre engine
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
#ifndef _OgreCEGUITexture_h_
#define _OgreCEGUITexture_h_

#include <CEGUI/CEGUIBase.h>
#include <CEGUI/CEGUIRenderer.h>
#include <CEGUI/CEGUITexture.h>
#include "OgreCEGUIRenderer.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Texture class that is created by OgreCEGUIRenderer objects
*/
class /*_OgrePrivate*/ OgreCEGUITexture : public Texture
{
private:
	/*************************************************************************
		Friends (to allow construction and destruction)
	*************************************************************************/
	friend	Texture* OgreCEGUIRenderer::createTexture(void);
	friend	Texture* OgreCEGUIRenderer::createTexture(const String& filename, const String& resourceGroup);
	friend	Texture* OgreCEGUIRenderer::createTexture(float size);
	friend	void	 OgreCEGUIRenderer::destroyTexture(Texture* texture);


	/*************************************************************************
		Construction & Destruction (by Renderer object only)
	*************************************************************************/
	OgreCEGUITexture(Renderer* owner);
	virtual ~OgreCEGUITexture(void);

public:
	/*!
	\brief
		Returns the current pixel width of the texture

	\return
		ushort value that is the current width of the texture in pixels
	*/
	virtual	ushort	getWidth(void) const		{constLoad(); return d_width;}


	/*!
	\brief
		Returns the current pixel height of the texture

	\return
		ushort value that is the current height of the texture in pixels
	*/
	virtual	ushort	getHeight(void) const		{constLoad(); return d_height;}


	/*!
	\brief
		Loads the specified image file into the texture.  The texture is resized as required to hold the image.

	\param filename
		The filename of the image file that is to be loaded into the texture

	\return
		Nothing.
	*/
	virtual void	loadFromFile(const String& filename, const String& resourceGroup);


	/*!
	\brief
		Loads (copies) an image in memory into the texture.  The texture is resized as required to hold the image.
	
	\param buffPtr
		Pointer to the buffer containing the image data
	
	\param buffWidth
		Width of the buffer (in pixels as specified by \a pixelFormat )
	
	\param buffHeight
		Height of the buffer (in pixels as specified by \a pixelFormat )
	
	\param pixelFormat
	PixelFormat value describing the format contained in \a buffPtr
	
	\return
		Nothing.
	*/
	virtual void	loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat);


	/*!
	\brief
		目前是用在文字更新上
	*/
	virtual void	updateRect( uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat );


	/*!
	\brief
		Return a pointer to the internal Ogre::Texture object

	\return
		Pointer to the Ogre::Texture object currently being used by this Texture object
	*/
	Ogre::TexturePtr	getOgreTexture(void);


	/*!
	\brief
		set the size of the internal Ogre texture.  Previous Ogre texture is lost.

	\param size
		pixel size of the new internal texture.  This will be rounded up to a power of 2.

	\return
		Nothing.
	*/
	void	setOgreTextureSize(uint size);


	/*!
	\brief
		Set the internal Ogre::TexturePtr object.

	\param texture
		Reference to an Ogre::TexturePtr object that is to be used by this Texture object.

	\return
		Nothing.
	*/
	void	setOgreTexture(Ogre::TexturePtr& texture);

	/*!
	\brief
		确保加载
	*/
	void	constLoad() const;

	/*!
	\brief
		是否能回收资源
	*/
	bool canRecycle();

	/*!
	\brief
		回收资源
	\param size
		回收的资源大小(字节)
	\return
		返回回收是否成功, 
	*/
	bool recycleTexture(size_t& size);

	/*!
	\brief
		获取上次访问资源的时间, 单位毫秒
	*/
	long getLastVisitTexTime() {return d_lastVisitTex;}

	size_t getTextureSize();

private:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	// safely free Ogre::Texture texture (can be called multiple times with no ill effect)
	void	freeOgreTexture(void);

	// return a Ogre::string that contains a unique name.
	Ogre::String	getUniqueName(void);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	static	uint32          d_texturenumber;	//!< Counter used to provide unique texture names.

	mutable Ogre::TexturePtr		d_ogre_texture;		//!< The 'real' texture.

	mutable ushort					d_width;			//!< cached width of the texture
	mutable ushort					d_height;			//!< cached height of the texture

	bool	d_isLinked;		//!< True if we are linked to a texture we did not actually create.

	mutable bool					d_delay;			//!< 延迟读取
	mutable std::string				d_delayLoadFile;	//!< 延迟读取的文件名
	mutable std::string				d_delayGroup;
	long							d_lastVisitTex;		//!< 上次访问材质的时间
};


} // End of  CEGUI namespace section


#endif	// end of guard _OgreCEGUITexture_h_

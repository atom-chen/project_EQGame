/******************************************************************************
* 作者: Y3
* 时间: 2010-5-25
* 描述: 地形纹理的XML解析类. 修改于TD
******************************************************************************/
#ifndef __TERRAINPAINTXMLHANDLER_H__
#define __TERRAINPAINTXMLHANDLER_H__

#include "TerrainPaintInfoContainer.h"


/** 地形纹理的XML解析类
*/
class TerrainPaintXMLHandler
{
public:

	explicit TerrainPaintXMLHandler(TerrainPaintInfoContainer *container);

	~TerrainPaintXMLHandler(void);

	void parseXMLFile(const std::string& xmlFile);


	void clearTextureInfos(void)
	{
		mPaintInfoContainer->clearTextureInfos();
	}



protected:

	void processTexture(TiXmlElement* node);

	void _pushErrorInfoString(const std::string& errorInfo);

	const std::string _getCurrentTextureInfo(void) const;

protected:

	TerrainPaintInfoContainer *mPaintInfoContainer;

	TextureInfo mCurrentTextureInfo; /// 保存当前这在处理的纹理的信息

};


#endif
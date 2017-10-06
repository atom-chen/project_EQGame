/******************************************************************************
* ����: Y3
* ʱ��: 2010-5-25
* ����: ���������XML������. �޸���TD
******************************************************************************/
#ifndef __TERRAINPAINTXMLHANDLER_H__
#define __TERRAINPAINTXMLHANDLER_H__

#include "TerrainPaintInfoContainer.h"


/** ���������XML������
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

	TextureInfo mCurrentTextureInfo; /// ���浱ǰ���ڴ�����������Ϣ

};


#endif
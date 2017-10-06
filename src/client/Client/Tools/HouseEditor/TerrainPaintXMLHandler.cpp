#include "HouseEditorPCH.h"

#include "TerrainPaintXMLHandler.h"

#include "XmlDataCenter.h"
#include "CommFunc.h"
#include "EQException.h"


TerrainPaintXMLHandler::TerrainPaintXMLHandler(TerrainPaintInfoContainer *container) : 
mPaintInfoContainer(container)
{
	mCurrentTextureInfo.clear();       
}

/** Destructor for SceneXMLHandler objects */
TerrainPaintXMLHandler::~TerrainPaintXMLHandler(void)
{
}

void TerrainPaintXMLHandler::parseXMLFile(const std::string& xmlFile)
{
	XmlDataTable x;
	x.loadResource( OpenResource( xmlFile ) );

	const size_t colOwnerTextName= x.getColumnIndex( "OwnerTextName" );		// ������ͼ����
	const size_t colBrushName	= x.getColumnIndex( "BrushName" );			// ��ˢ����
	const size_t colTexSize		= x.getColumnIndex( "TexSize" );			// ��ͼ��С
	const size_t colTexUV		= x.getColumnIndex( "TexUV" );				// ��ͼ����
	const size_t colTexType		= x.getColumnIndex( "TexType" );			// ��ͼ����
	const size_t colTexTypeInfo	= x.getColumnIndex( "TexTypeInfo" );		// �������ͼ������Ϣ
	const size_t colRotDir		= x.getColumnIndex( "RotDir" );				// ��ת����

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{
		mCurrentTextureInfo.clear();

		{// ������ͼ����
			std::string name = x.cell(iRow, colOwnerTextName, true);
			if ( name.empty() )
				EQ_EXCEPT("OwnerTextName is null!", "TerrainPaintXMLHandler::parseXMLFile");
			mCurrentTextureInfo.ownerTextureName = name;
		}
		{// ��ˢ����
			std::string name = x.cell(iRow, colBrushName, true);
			mCurrentTextureInfo.brushName = name;
		}
		{// ��ͼ��С
			std::string attValue = x.cell(iRow, colTexSize);
			Ogre::StringVector valueVector = Ogre::StringUtil::split(attValue);
			if ( valueVector.size() != 2 )
				EQ_EXCEPT( "'" + mCurrentTextureInfo.ownerTextureName + "' TexSize error!", "TerrainPaintXMLHandler::parseXMLFile");
			mCurrentTextureInfo.width = Ogre::StringConverter::parseUnsignedInt(valueVector[0]);
			mCurrentTextureInfo.height = Ogre::StringConverter::parseUnsignedInt(valueVector[1]);
		}
		{// ��ͼ����
			std::string attValue = x.cell(iRow, colTexUV);
			Ogre::StringVector valueVector = Ogre::StringUtil::split(attValue);
			if ( valueVector.size() != 2 )
				EQ_EXCEPT( "'" + mCurrentTextureInfo.ownerTextureName + "' TexUV error!", "TerrainPaintXMLHandler::parseXMLFile");
			mCurrentTextureInfo.leftCorner = Ogre::StringConverter::parseUnsignedInt(valueVector[0]);
			mCurrentTextureInfo.topCorner = Ogre::StringConverter::parseUnsignedInt(valueVector[1]);
		}
		{// ��ͼ����
			std::string attValue = x.cell(iRow, colTexType, true);
			TerrainPaintInfoContainer::StringTexTypeMap::const_iterator it = mPaintInfoContainer->mStringTexTypeMap.find(attValue);
			if  ( it == mPaintInfoContainer->mStringTexTypeMap.end() )
				EQ_EXCEPT(attValue + " can't find the texture type!", "TerrainPaintXMLHandler::parseXMLFile");
			// �Ӷ�Ӧ���в��ҳ���Ӧ�������ַ���
			mCurrentTextureInfo.textureType = it->second;
		}
		{// �������ͼ������Ϣ
			std::string attValue = x.cell(iRow, colTexTypeInfo, true);
			TerrainPaintInfoContainer::StringExtraTexTypeMap::const_iterator itt = mPaintInfoContainer->mStringExtraTexTypeMap.find(attValue);
			if ( itt == mPaintInfoContainer->mStringExtraTexTypeMap.end() )
				EQ_EXCEPT(attValue + " can't find the extra texture info type!", "TerrainPaintXMLHandler::parseXMLFile");
			// �Ӷ�Ӧ���в��ҳ���Ӧ�������ַ���
			mCurrentTextureInfo.extraTexType = itt->second;
		}
		{// ��ת����
			std::string attValue = x.cell(iRow, colRotDir);
			mCurrentTextureInfo.rotateType = Ogre::StringConverter::parseUnsignedInt(attValue);
			// ��ƴ�ӵĴ���ͼ�з���ֻ����4��ѡ��
			if ( false == (mCurrentTextureInfo.rotateType >= 0 && mCurrentTextureInfo.rotateType <= 3) )
				EQ_EXCEPT(attValue + " rotate type must be between 0 and 3!", "TerrainPaintXMLHandler::parseXMLFile");
		}

		mPaintInfoContainer->addTextureInfo(mCurrentTextureInfo);
	}
}



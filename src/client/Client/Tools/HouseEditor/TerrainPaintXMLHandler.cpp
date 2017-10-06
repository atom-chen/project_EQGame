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

	const size_t colOwnerTextName= x.getColumnIndex( "OwnerTextName" );		// 所属贴图名称
	const size_t colBrushName	= x.getColumnIndex( "BrushName" );			// 画刷名称
	const size_t colTexSize		= x.getColumnIndex( "TexSize" );			// 贴图大小
	const size_t colTexUV		= x.getColumnIndex( "TexUV" );				// 贴图坐标
	const size_t colTexType		= x.getColumnIndex( "TexType" );			// 贴图类型
	const size_t colTexTypeInfo	= x.getColumnIndex( "TexTypeInfo" );		// 额外的贴图类型信息
	const size_t colRotDir		= x.getColumnIndex( "RotDir" );				// 旋转方向

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{
		mCurrentTextureInfo.clear();

		{// 所属贴图名称
			std::string name = x.cell(iRow, colOwnerTextName, true);
			if ( name.empty() )
				EQ_EXCEPT("OwnerTextName is null!", "TerrainPaintXMLHandler::parseXMLFile");
			mCurrentTextureInfo.ownerTextureName = name;
		}
		{// 画刷名称
			std::string name = x.cell(iRow, colBrushName, true);
			mCurrentTextureInfo.brushName = name;
		}
		{// 贴图大小
			std::string attValue = x.cell(iRow, colTexSize);
			Ogre::StringVector valueVector = Ogre::StringUtil::split(attValue);
			if ( valueVector.size() != 2 )
				EQ_EXCEPT( "'" + mCurrentTextureInfo.ownerTextureName + "' TexSize error!", "TerrainPaintXMLHandler::parseXMLFile");
			mCurrentTextureInfo.width = Ogre::StringConverter::parseUnsignedInt(valueVector[0]);
			mCurrentTextureInfo.height = Ogre::StringConverter::parseUnsignedInt(valueVector[1]);
		}
		{// 贴图坐标
			std::string attValue = x.cell(iRow, colTexUV);
			Ogre::StringVector valueVector = Ogre::StringUtil::split(attValue);
			if ( valueVector.size() != 2 )
				EQ_EXCEPT( "'" + mCurrentTextureInfo.ownerTextureName + "' TexUV error!", "TerrainPaintXMLHandler::parseXMLFile");
			mCurrentTextureInfo.leftCorner = Ogre::StringConverter::parseUnsignedInt(valueVector[0]);
			mCurrentTextureInfo.topCorner = Ogre::StringConverter::parseUnsignedInt(valueVector[1]);
		}
		{// 贴图类型
			std::string attValue = x.cell(iRow, colTexType, true);
			TerrainPaintInfoContainer::StringTexTypeMap::const_iterator it = mPaintInfoContainer->mStringTexTypeMap.find(attValue);
			if  ( it == mPaintInfoContainer->mStringTexTypeMap.end() )
				EQ_EXCEPT(attValue + " can't find the texture type!", "TerrainPaintXMLHandler::parseXMLFile");
			// 从对应表中查找出相应的类型字符串
			mCurrentTextureInfo.textureType = it->second;
		}
		{// 额外的贴图类型信息
			std::string attValue = x.cell(iRow, colTexTypeInfo, true);
			TerrainPaintInfoContainer::StringExtraTexTypeMap::const_iterator itt = mPaintInfoContainer->mStringExtraTexTypeMap.find(attValue);
			if ( itt == mPaintInfoContainer->mStringExtraTexTypeMap.end() )
				EQ_EXCEPT(attValue + " can't find the extra texture info type!", "TerrainPaintXMLHandler::parseXMLFile");
			// 从对应表中查找出相应的类型字符串
			mCurrentTextureInfo.extraTexType = itt->second;
		}
		{// 旋转方向
			std::string attValue = x.cell(iRow, colRotDir);
			mCurrentTextureInfo.rotateType = Ogre::StringConverter::parseUnsignedInt(attValue);
			// 在拼接的大贴图中方向只能有4种选择
			if ( false == (mCurrentTextureInfo.rotateType >= 0 && mCurrentTextureInfo.rotateType <= 3) )
				EQ_EXCEPT(attValue + " rotate type must be between 0 and 3!", "TerrainPaintXMLHandler::parseXMLFile");
		}

		mPaintInfoContainer->addTextureInfo(mCurrentTextureInfo);
	}
}



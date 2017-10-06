#include "EQMainPCH.h"

#include "GraphicsTable.h"
#include "CommFunc.h"
#include "CommFuncGcs.h"
#include "XmlDataCenter.h"


//-----------------------------------------------------------------------
template<> GraphicsTableManager* Ogre::Singleton<GraphicsTableManager>::ms_Singleton = 0;

//-----------------------------------------------------------------------
void GraphicsTableManager::load()
{
	loadActiveLightTable();
	loadPassLightTable();
	loadLightTabel();
	loadModelDisplayTable();
	loadAnimationBlendTable();
	loadParticleTable();
	loadSceneEntityObjTable();
}
//-----------------------------------------------------------------------
const ModelDisplayRow *	GraphicsTableManager::getModelDisplayRow( uint id ) const
{
	ModelDisplayTable::const_iterator it = mModelDisplayTable.find( id );
	if ( it == mModelDisplayTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const ActiveLightRow* GraphicsTableManager::getActiveLightRow(uint id) const
{
	ActiveLightTable::const_iterator it = mActiveLightTable.find( id );
	if ( it == mActiveLightTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const PassLightRow* GraphicsTableManager::getPassLightRow(uint id) const
{
	PassLightTable::const_iterator it = mPassLightTable.find( id );
	if ( it == mPassLightTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const LightRow* GraphicsTableManager::getLightRow(uint id) const
{
	LightTable::const_iterator it = mLightTable.find( id );
	if ( it == mLightTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
const ParticleRow* GraphicsTableManager::getParticleRow(uint id) const
{
	ParticleTable::const_iterator it = mParticleTable.find( id );
	if ( it == mParticleTable.end() )
		return NULL;
	return &it->second;
}
//-----------------------------------------------------------------------
void GraphicsTableManager::loadModelDisplayTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "ModelDisplay.xml" ) );

	const size_t colID			= x.getColumnIndex( "ID" );
	const size_t colMesh		= x.getColumnIndex( "Mesh" );
	const size_t colMat			= x.getColumnIndex( "Mat" );
	const size_t colShowMark	= x.getColumnIndex( "ShowMark" );
	const size_t colShowPri		= x.getColumnIndex( "ShowPri" );
	const size_t colParticle	= x.getColumnIndex( "particle" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ModelDisplayRow r;
		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.mesh		= x.cell( iRow, colMesh, true );
		r.mat		= x.cell( iRow, colMat, true );

		r.showMark	= strtoul( x.cell( iRow, colShowMark, true ).c_str(), NULL, 16 );
		r.showPri	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colShowPri ) );
		r.particle	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colParticle ) );

		if ( mModelDisplayTable.find( r.id ) != mModelDisplayTable.end() )
			EQ_EXCEPT( FormatStr("ModelDisPlay.xml ID %d Repeat", r.id), "loadModelDisplayTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "ModelDisPlay.xml ID null", "loadModelDisplayTable" );			

		if ( r.mesh.empty() )
			EQ_EXCEPT( "ModelDisPlay.xml Mesh null", "loadModelDisplayTable" );			

		mModelDisplayTable.insert( std::make_pair( r.id, r ) );
	}	
}
//-----------------------------------------------------------------------
void GraphicsTableManager::loadActiveLightTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "ActiveLight.xml" ) );

	const size_t colID			= x.getColumnIndex( "ID" );
	const size_t colName		= x.getColumnIndex( "Name" );
	const size_t colLightType	= x.getColumnIndex( "LightType" );
	const size_t colDiffCol		= x.getColumnIndex( "DiffCol" );
	const size_t colSpecCol		= x.getColumnIndex( "SpecCol" );
	const size_t colAttenuation	= x.getColumnIndex( "Attenuation" );
	const size_t colPos			= x.getColumnIndex( "Pos" );
	const size_t colDir			= x.getColumnIndex( "Dir" );
	const size_t colSpotRange	= x.getColumnIndex( "SpotRange" );
	const size_t colPierce		= x.getColumnIndex( "Pierce" );
	const size_t colPierceRange	= x.getColumnIndex( "PierceRange" );
	const size_t colBbsTexture	= x.getColumnIndex( "BbsTexture" );
	const size_t colBbsPos		= x.getColumnIndex( "BbsPos" );
	const size_t colBbsType		= x.getColumnIndex( "BbsType" );

	std::string str;
	std::vector<std::string> strArr;
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ActiveLightRow r;
		r.ID			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.Name			= x.cell( iRow, colName, true );
		r.LightType		= (uchar)Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLightType ) );

		const std::string& strDiff = x.cell( iRow, colDiffCol );
		if ( strDiff.empty() )
			r.DiffCol = Ogre::ColourValue::White;		//默认为白色
		else
			r.DiffCol = extractColorFromString(strDiff);

		const std::string& strSpec = x.cell( iRow, colSpecCol );
		if ( strSpec.empty() )
			r.SpecCol = Ogre::ColourValue::Black;		//默认为黑色
		else
			r.SpecCol = extractColorFromString(strSpec);

		Ogre::Vector4 tmpAtt = Ogre::StringConverter::parseVector4(x.cell( iRow, colAttenuation ));
		r.Attenuation[0] = tmpAtt.x;
		r.Attenuation[1] = tmpAtt.y;
		r.Attenuation[2] = tmpAtt.z;
		r.Attenuation[3] = tmpAtt.w;

		r.Pos = Ogre::StringConverter::parseVector3(x.cell( iRow, colPos ));
		r.Dir = Ogre::StringConverter::parseVector3(x.cell( iRow, colDir ));

		const std::string& strSpot =  x.cell( iRow, colSpotRange );
		if ( strSpot.empty() )
		{
			r.SpotRange[0] = 30;						//默认为30, 40, 1
			r.SpotRange[1] = 40;
			r.SpotRange[2] = 1;
		}
		else
		{
			Ogre::Vector3 tmpSpot = Ogre::StringConverter::parseVector3(strSpot);
			r.SpotRange[0] = tmpSpot.x;
			r.SpotRange[1] = tmpSpot.y;
			r.SpotRange[2] = tmpSpot.z;
		}

		r.Pierce = (uchar)Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPierce ) );
		r.PierceRange = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPierceRange ) );

		r.BbsTexture	= x.cell( iRow, colBbsTexture, true );

		str				= x.cell( iRow, colBbsPos, true );
		Ogre::StringUtil::trim( str );
		if ( !str.empty() )
		{
			strArr = Ogre::StringUtil::split( str, " \r\n\t()" );
			for ( std::vector<std::string>::iterator it = strArr.begin();
				it != strArr.end();
				++it )
			{
				r.BbsPos.push_back(StringToVector3(*it));
			}
		}

		r.BbsType = Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colBbsType ) );

		if ( mActiveLightTable.find( r.ID ) != mActiveLightTable.end() )
			EQ_EXCEPT( FormatStr("ActivelLight.xml ID %d Repeat", r.ID), "loadActiveLightTable" );

		if ( 0 == r.ID )
			EQ_EXCEPT( "ActivelLight.xml ID null", "loadActiveLightTable" );

		if ( r.LightType != Ogre::Light::LT_POINT && r.LightType != Ogre::Light::LT_SPOTLIGHT )
			EQ_EXCEPT( "ActivelLight.xml LightType invaild", "loadActiveLightTable" );


		mActiveLightTable.insert( std::make_pair( r.ID, r ) );
	}	
}
//-----------------------------------------------------------------------
void GraphicsTableManager::loadPassLightTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "PassLight.xml" ) );

	const size_t colID			= x.getColumnIndex( "ID" );
	const size_t colName		= x.getColumnIndex( "Name" );
	const size_t colIndoor		= x.getColumnIndex( "Indoor" );
	const size_t colDayAtten	= x.getColumnIndex( "Atten_Day" );
	const size_t colNightAtten	= x.getColumnIndex( "Atten_Night" );
	const size_t colPos			= x.getColumnIndex( "Pos" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		PassLightRow r;
		r.ID		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.Name		= x.cell( iRow, colName, true );
		r.Indoor	= Ogre::StringConverter::parseBool(x.cell(iRow, colIndoor));

		Ogre::Vector4 tmpAtt = Ogre::StringConverter::parseVector4(x.cell( iRow, colDayAtten ));
		r.DayAtten[0] = tmpAtt.x;
		r.DayAtten[1] = tmpAtt.y;
		r.DayAtten[2] = tmpAtt.z;
		r.DayAtten[3] = tmpAtt.w;

		tmpAtt = Ogre::StringConverter::parseVector4(x.cell( iRow, colNightAtten ));
		r.NightAtten[0] = tmpAtt.x;
		r.NightAtten[1] = tmpAtt.y;
		r.NightAtten[2] = tmpAtt.z;
		r.NightAtten[3] = tmpAtt.w;

		r.Pos = Ogre::StringConverter::parseVector3(x.cell( iRow, colPos ));

		//
		if ( mPassLightTable.find( r.ID ) != mPassLightTable.end() )
			EQ_EXCEPT( FormatStr("PassLightTable.xml ID %d Repeat", r.ID), "loadPassLightTable" );

		if ( 0 == r.ID )
			EQ_EXCEPT( "PassLightTable.xml ID null", "loadPassLightTable" );


		mPassLightTable.insert( std::make_pair( r.ID, r ) );
	}	
}
//-----------------------------------------------------------------------
void GraphicsTableManager::loadLightTabel()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "Light.xml" ) );

	const size_t colID			= x.getColumnIndex( "ID" );
	const size_t colName		= x.getColumnIndex( "Name" );
	const size_t colVisible		= x.getColumnIndex( "Visible" );
	const size_t colLight1		= x.getColumnIndex( "Light1" );
	const size_t colLight2		= x.getColumnIndex( "Light2" );
	const size_t colPassLi1		= x.getColumnIndex( "PassLi1" );
	const size_t colPassLi2		= x.getColumnIndex( "PassLi2" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		LightRow r;
		r.ID		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.Name		= x.cell( iRow, colName, true );

		const std::string& strVis = x.cell(iRow, colVisible);
		if ( strVis.empty() )
			r.Visible = true;
		else
			r.Visible	= Ogre::StringConverter::parseBool(strVis);

		r.Light1 = getActiveLightRow(Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLight1 ) ));
		r.Light2 = getActiveLightRow(Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLight2 ) ));
		r.PassLi1 = getPassLightRow(Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPassLi1 ) ));
		r.PassLi2 = getPassLightRow(Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colPassLi2 ) ));

		//
		if ( mLightTable.find( r.ID ) != mLightTable.end() )
			EQ_EXCEPT( FormatStr("LightTabel.xml ID  %d Repeat", r.ID), "loadLightTabel" );

		if ( 0 == r.ID )
			EQ_EXCEPT( "LightTabel.xml ID null", "loadLightTabel" );


		mLightTable.insert( std::make_pair( r.ID, r ) );
	}	
}
//-----------------------------------------------------------------------
void GraphicsTableManager::loadAnimationBlendTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "animation_blend.xml" ) );

	if(x.getRowCount() != x.getColumnCount())
		EQ_EXCEPT( "animation_blend.xml format invalid : row and column count unequal!", "loadLightTabel" );

	std::string rowName,colName;
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{	
		rowName = x.columns()[iRow];
		for( size_t iCol = 0; iCol < x.getColumnCount(); ++iCol )
		{
			if(iCol == iRow)
				continue;

			colName = x.columns()[iCol];
			float value = Ogre::StringConverter::parseReal(x.cell(iRow, iCol));

			mAnimationBlendTable.insert(std::make_pair(colName+rowName, value));
		}
	}	
}
//-----------------------------------------------------------------------
float GraphicsTableManager::getAnimationBlend(std::string& key)
{
	AnimationBlendTable::iterator it = mAnimationBlendTable.find(key);
	if(it == mAnimationBlendTable.end())
		return 0;
	else
		return it->second;
}
//-----------------------------------------------------------------------
void GraphicsTableManager::loadParticleTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "particle.xml" ) );

	const size_t colID			= x.getColumnIndex( "id" );
	const size_t colName		= x.getColumnIndex( "name" );
	const size_t colTemplate	= x.getColumnIndex( "template" );
	const size_t colLife		= x.getColumnIndex( "life" );
	const size_t colDelay		= x.getColumnIndex( "delay" );
	const size_t colPos			= x.getColumnIndex( "pos" );
	const size_t colRot			= x.getColumnIndex( "rot" );
	const size_t colScale		= x.getColumnIndex( "scale" );
	const size_t colBone		= x.getColumnIndex( "bone" );
	const size_t colTrans		= x.getColumnIndex( "trans" );

	std::string str;
	std::vector<std::string> strArr;
	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		ParticleRow r;
		r.id			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.name			= x.cell( iRow, colName, true );
		r.templateName	= x.cell( iRow, colTemplate, true );
		r.life			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLife ) );
		r.delay			= Ogre::StringConverter::parseInt( x.cell( iRow, colDelay ) );
		r.bone			= x.cell( iRow, colBone, true );
		r.trans			= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colTrans ) );

		str				= x.cell( iRow, colPos, true );
		Ogre::StringUtil::trim( str );
		if ( str.empty() )
		{
			r.pos = Ogre::Vector3( 0, 0, 0 );
		}
		else
		{
			strArr = Ogre::StringUtil::split( str, "\t ," );
			if ( strArr.size() != 3 )
				EQ_EXCEPT( "particle.xml Pos data error", "GraphicsTableManager::loadParticleTable" );

			r.pos.x = Ogre::StringConverter::parseReal( strArr[0] );
			r.pos.y = Ogre::StringConverter::parseReal( strArr[1] );
			r.pos.z = Ogre::StringConverter::parseReal( strArr[2] );
		}

		str				= x.cell( iRow, colRot, true );
		Ogre::StringUtil::trim( str );
		if ( str.empty() )
		{
			r.rot = Ogre::Quaternion::IDENTITY;
		}
		else
		{
			strArr = Ogre::StringUtil::split( str, "\t ," );
			if ( strArr.size() != 4 )
				EQ_EXCEPT( "particle.xml Rot data error", "GraphicsTableManager::loadParticleTable" );
			r.rot.x = Ogre::StringConverter::parseReal( strArr[0] );
			r.rot.y = Ogre::StringConverter::parseReal( strArr[1] );
			r.rot.z = Ogre::StringConverter::parseReal( strArr[2] );
			r.rot.w = Ogre::StringConverter::parseReal( strArr[3] );
			if(r.rot.x==0 && r.rot.y==0 && r.rot.z==0 && r.rot.w==0)
				EQ_EXCEPT( "particle.xml Rot data invalid", "GraphicsTableManager::loadParticleTable" );
		}

		str				= x.cell( iRow, colScale, true );
		Ogre::StringUtil::trim( str );
		if ( str.empty() )
		{
			r.pos = Ogre::Vector3( 0, 0, 0 );
		}
		else
		{
			strArr = Ogre::StringUtil::split( str, "\t ," );
			if ( strArr.size() != 3 )
				EQ_EXCEPT( "particle.xml Pos data error", "GraphicsTableManager::loadParticleTable" );

			r.scale.x = Ogre::StringConverter::parseReal( strArr[0] );
			r.scale.y = Ogre::StringConverter::parseReal( strArr[1] );
			r.scale.z = Ogre::StringConverter::parseReal( strArr[2] );
		}

		if ( mParticleTable.find( r.id ) != mParticleTable.end() )
			EQ_EXCEPT( FormatStr("particle.xml ID %d Repeat", r.id), "loadParticleTable" );

		if ( 0 == r.id )
			EQ_EXCEPT( "particle.xml ID null", "loadParticleTable" );

		mParticleTable.insert( std::make_pair( r.id, r ) );
	}	
}
//----------------------------------------------------------------
bool SceneEntityObjRowMore(SceneEntityObjRow& a, SceneEntityObjRow& b)
{
	if ( a.mesh < b.mesh )
		return true;
	else if ( a.mesh > b.mesh )
		return false;

	if ( a.material < b.material )
		return true;
	else if ( a.material > b.material )
		return false;

	return false;
}
//----------------------------------------------------------------
const SceneEntityObjRow* GraphicsTableManager::getSceneEntityObjRow(uint id) const
{
	SceneEntityObjMapTable::const_iterator it = mSceneEntityObjMapTable.find( id );
	if ( it == mSceneEntityObjMapTable.end() )
		return NULL;
	return &it->second;
}
//----------------------------------------------------------------
void GraphicsTableManager::loadSceneEntityObjTable()
{
	XmlDataTable x;
	x.loadResource( OpenResource( "SceneEntityObj.xml" ) );

	const size_t colID				= x.getColumnIndex( "ID" );
	const size_t colMesh			= x.getColumnIndex( "Mesh" );
	const size_t colMaterial		= x.getColumnIndex( "Material" );
	const size_t colLight			= x.getColumnIndex( "Light" );
	const size_t colCutLevel		= x.getColumnIndex( "CutLevel" );
	const size_t colParticle		= x.getColumnIndex( "Particle" );
	const size_t colLoop			= x.getColumnIndex( "Loop" );

	for ( size_t iRow = 0; iRow < x.getRowCount(); ++iRow )
	{		
		SceneEntityObjRow r;
		r.id		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colID ) );
		r.mesh		= x.cell( iRow, colMesh, true );
		r.material	= x.cell( iRow, colMaterial, true );
		r.light		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLight ) );
		r.cutLevel	= x.cell( iRow, colCutLevel);
		r.particle	= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colParticle ) );
		r.loop		= Ogre::StringConverter::parseUnsignedLong( x.cell( iRow, colLoop ) );

		if ( r.mesh.empty() )	//如果没有模型, 则跳过
			continue;

		if ( !(r.cutLevel == "" || r.cutLevel == "f" || r.cutLevel == "m"  || r.cutLevel == "c") )
			EQ_EXCEPT("CutLevelcolum value invaild!", "GraphicsTableManager::loadSceneEntityObjTable");

		if ( r.id == 0)
			mSceneEntityObjTable.push_back(r);
		else
			mSceneEntityObjMapTable[r.id] = r;
	}	

	//排序
	std::sort(mSceneEntityObjTable.begin(), mSceneEntityObjTable.end(), SceneEntityObjRowMore);
}

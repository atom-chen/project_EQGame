#include "EQMainPCH.h"

#include "HouseSceneFloor.h"
#include "HouseScene.h"
#include "HouseTable.h"
#include "SystemGcs.h"
#include "Ogre/OgreFontManager.h"

namespace
{
	/**
		画数字辅助类
	*/
	class NumberQuadBuilder
	{
	public:
		NumberQuadBuilder( Ogre::SceneManager * mgr )
			: mIdx(0)
			, mObj(NULL)
			, mSceneMgr(mgr)
		{
		}
	public:
		void begin()
		{
			assert( !mIdx );
			assert( !mObj );

			mFont = Ogre::FontManager::getSingleton().getByName( "BlueHighway" );
			mFont->load();
			Ogre::MaterialPtr mpMaterial = mFont->getMaterial();
			mpMaterial->setDepthCheckEnabled(false);
			mpMaterial->setLightingEnabled(false);
			mpMaterial->setCullingMode(Ogre::CULL_NONE);	

			static int num = 0;
			++num;
			std::string name = "--RoomIDPlane--[auto]--" + Ogre::StringConverter::toString(num) + "--";
			mObj = mSceneMgr->createManualObject(name);
			mObj->begin(mpMaterial->getName().c_str(), Ogre::RenderOperation::OT_TRIANGLE_LIST);		
		}
	public:
		Ogre::ManualObject * end()
		{
			mObj->end();
			return mObj;
		}
	public:
		void drawNumber( int x, int z, int hx, int hz, Ogre::uint number )
		{
			char temp[2];
			if( number < 10 )
			{
				sprintf_s(temp, 2, "%u", number);
				const Ogre::Font::UVRect& uvRect1 = mFont->getGlyphTexCoords(Ogre::DisplayString(temp).begin().getCharacter());

				_buildNumberQuad( x, z, hx, hz, uvRect1 );
			}
			else
			{
				sprintf_s(temp, 2, "%u", number%100/10);
				const Ogre::Font::UVRect& uvRect1 = mFont->getGlyphTexCoords(Ogre::DisplayString(temp).begin().getCharacter());

				sprintf_s(temp, 2, "%u", number%10);
				const Ogre::Font::UVRect& uvRect2 = mFont->getGlyphTexCoords(Ogre::DisplayString(temp).begin().getCharacter());

				_buildNumberQuad( x - hx/2, z, hx/2, hz, uvRect1 );
				_buildNumberQuad( x + hx/2, z, hx/2, hz, uvRect2 );
			}

		}
	private:
		void _buildNumberQuad( int x, int z, int hx, int hz, const Ogre::Font::UVRect & rc )
		{
			mObj->position(x - hx, 0, z - hz);
			mObj->textureCoord(rc.left, rc.top);
			mObj->position(x - hx, 0, z + hz);
			mObj->textureCoord(rc.left, rc.bottom-0.015);
			mObj->position(x + hx, 0, z + hz);
			mObj->textureCoord(rc.right, rc.bottom-0.015);
			mObj->position(x + hx, 0, z - hz);
			mObj->textureCoord(rc.right, rc.top);

			mObj->quad(mIdx, mIdx + 1, mIdx + 2, mIdx + 3);
			mIdx += 4;
		}
	private:
		Ogre::ManualObject *	mObj;
		int						mIdx;
		Ogre::FontPtr			mFont;
		Ogre::SceneManager *	mSceneMgr;
	};


	/**
		画长方形辅助类
	*/
	class QuadBuilder
	{
	public:
		explicit QuadBuilder( Ogre::ManualObject * obj, float y )
			: mIdx( 0 )
			, mObj( obj )
			, mY( y )
		{
			assert( obj );
		}
	public:
		/**
			@remark centerX, centerZ 中心点
					halfX, halfZ为半长半宽
		*/
		void drawQuad( float centerX, float centerZ, float halfX, float halfZ, const Ogre::ColourValue & colour )
		{
			mObj->position(centerX - halfX,	mY, centerZ - halfZ);
			mObj->colour(colour);

			mObj->position(centerX + halfX,	mY, centerZ - halfZ);

			mObj->position(centerX + halfX,	mY, centerZ + halfZ );

			mObj->position(centerX - halfX,	mY, centerZ + halfZ);

			mObj->quad( mIdx, mIdx + 3, mIdx + 2, mIdx + 1 );

			// 画了4点
			mIdx += 4;
		}
	private:
		float					mY;
		int						mIdx;
		Ogre::ManualObject *	mObj;	
	};


	//-----------------------------------------------------------------------
	/** 画墙辅助函数
	*/
	static void _drawWall( QuadBuilder & qb, const HouseSceneGrid * gr )
	{
		const int dis = HOUSE_GRID_HALF_SIZE - 3;
		// 取中心点
		const int centerX = gr->getGridX() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE;
		const int centerZ = gr->getGridZ() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE;
		// 红色
		const Ogre::ColourValue col(1,0,0,1);

		for ( int i = 0; i < WALL_FACE_COUNT; i++ )
		{
			if ( gr->getWallFace(i) )
			{
		
				if ( i == WALL_FACE_X )
				{
					qb.drawQuad( centerX + dis, centerZ, 3, HOUSE_GRID_HALF_SIZE - 3, col );
				}
				else if ( i == WALL_FACE_NX )
				{
					qb.drawQuad( centerX - dis, centerZ, 3, HOUSE_GRID_HALF_SIZE - 3, col );
				}
				else if ( i == WALL_FACE_NZ )
				{
					qb.drawQuad( centerX, centerZ - dis, HOUSE_GRID_HALF_SIZE - 3, 3, col );
				}
				else if ( i == WALL_FACE_Z )
				{
					qb.drawQuad( centerX, centerZ + dis, HOUSE_GRID_HALF_SIZE - 3, 3, col );
				}
			}
		}
	}


	//-----------------------------------------------------------------------
	static void _destroyManualObject( Ogre::ManualObject * obj )
	{
		if ( !obj )
			return;

		Ogre::SceneNode * node = obj->getParentSceneNode();
		node->detachObject( obj );
		node->getCreator()->destroyManualObject( obj );
	}
}

//-----------------------------------------------------------------------
HouseSceneFloor::HouseSceneFloor(HouseScene* scene) 
	: mCreator(scene)
	, mRootGrid(NULL)
	, mFloorHeight(0)
	, mUpFloor(NULL)
	, mDownFloor(NULL)
	, mFloorNO(0)
	, mGridTypePlane(NULL)
	, mWallPlane(NULL)
	, mHoldGridPlane(NULL)
	, mRoomIDPlane(NULL)
	, mSceneNode(NULL)
{
	mRootGrid = new HouseSceneGrid(this);
	mRootGrid->setGridType(HGT_FRAME);
	mRootGrid->frameGrid(true);

	mSceneNode = scene->getSceneNode()->createChildSceneNode();
}
//-----------------------------------------------------------------------
HouseSceneFloor::~HouseSceneFloor()
{
	if ( mRootGrid )
	{
		delete mRootGrid;
		mRootGrid = NULL;
	}

	{ //mGridMap
		GridMap::iterator it;
		for ( it = mGridMap.begin(); it != mGridMap.end(); ++it )
		{
			delete it->second;
		}
		mGridMap.clear();
	}

	_destroyManualObject( mGridTypePlane );
	_destroyManualObject( mWallPlane );
	_destroyManualObject( mHoldGridPlane );
	_destroyManualObject( mRoomIDPlane );
	
	mSceneNode->getParentSceneNode()->removeAndDestroyChild( mSceneNode->getName() );
}
//-----------------------------------------------------------------------
void HouseSceneFloor::calcWatchMode( HouseWatchMode mode )
{
	mRootGrid->calcWatchMode(mode);

	GridMap::iterator it;
	for ( it = mGridMap.begin(); it != mGridMap.end(); ++it )
	{
		it->second->calcWatchMode(mode);
	}
}
//-----------------------------------------------------------------------
int HouseSceneFloor::getHeight() const
{
	if ( this->mUpFloor )
		return mUpFloor->getFloorHeight() - this->getFloorHeight();

	return HOUSE_DEF_FLOOR_HEGHT;
}
//-----------------------------------------------------------------------
void HouseSceneFloor::modifyFloorHeight( float h )
{
	if ( hasGrids() )
	{
		EQ_EXCEPT( "Can't modify floor height! Because of having Grids!",
			"HouseSceneFloor::modifyFloorHeight" );
	}

	mFloorHeight = h;

	Ogre::Vector3 pos = getCreator()->getSceneNode()->getPosition() + Ogre::Vector3( 0, mFloorHeight, 0 );

	mSceneNode->setPosition( pos );
}
//-----------------------------------------------------------------------
void HouseSceneFloor::notifyGridChanged()
{
	_showGridTypePlane( mGridTypePlane && mGridTypePlane->getVisible() );

	_showWallPlane( mWallPlane && mWallPlane->getVisible() );

	_showHoldGridPlane( mHoldGridPlane && mHoldGridPlane->getVisible() );
}

void HouseSceneFloor::notifyRoomIDChanged()
{
	showRoomIDPlane( mRoomIDPlane && mRoomIDPlane->getVisible() );
}

void HouseSceneFloor::calcGridDisplayMode(GridDisplayMode mode)
{
	// 隐藏相关平面
	_showGridTypePlane(false);
	_showWallPlane(false);
	_showHoldGridPlane(false);
	showRoomIDPlane( false );

	// 根据情况显示
	switch(mode)
	{
	case GD_MODE_HIDE:
		break;
	case GD_MODE_GAME:
		break;
	case GD_MODE_FITMENT:
		_showHoldGridPlane( true, true );
		break;
	case GD_MODE_STYLE:
		_showGridTypePlane( true );
		_showHoldGridPlane( true );
		break;
	case GD_MODE_CONNECTEDNESS:
		_showGridTypePlane( true );
		_showHoldGridPlane( true );
		_showWallPlane( true );
		showRoomIDPlane( true );
		break;
	}
}
//-----------------------------------------------------------------------
void HouseSceneFloor::_showRoomIDPlane( bool bShow, bool bCareID, int filterID )
{
	_destroyManualObject( mRoomIDPlane );
	mRoomIDPlane = NULL;
	if ( !bShow )
		return;

	NumberQuadBuilder nqb( mSceneNode->getCreator() );
	nqb.begin();

	for ( GridMap::iterator it = mGridMap.begin(); it != mGridMap.end(); ++it )
	{
		HouseSceneGrid * gr = it->second;
		if ( bCareID &&	gr->getRoomID() != filterID )
			continue;

		const int x = gr->getGridX() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE;
		const int z = gr->getGridZ() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE;
		nqb.drawNumber( x, z, HOUSE_GRID_HALF_SIZE - 15, HOUSE_GRID_HALF_SIZE - 15, gr->getRoomID() );
	}

	mRoomIDPlane = nqb.end();

	mRoomIDPlane->setQueryFlags( 0 );
	mRoomIDPlane->setVisible( bShow );

	mSceneNode->attachObject( mRoomIDPlane );
}
//-----------------------------------------------------------------------
void HouseSceneFloor::showRoomIDPlane( bool bShow )
{
	_showRoomIDPlane( bShow, false, 0 );
}
//-----------------------------------------------------------------------
const HouseSceneGrid * HouseSceneFloor::getRootGrid() const
{
	return mRootGrid;
}
//-----------------------------------------------------------------------
HouseSceneGrid * HouseSceneFloor::getRootGrid()
{
	return mRootGrid;
}
//-----------------------------------------------------------------------
HouseSceneGrid* HouseSceneFloor::createGrid( int x, int z, ushort typeID, ushort roomID )
{
	if ( hasGrid(x,z) )
		EQ_EXCEPT("Grid is exist!", "HouseSceneFloor::createGrid");

	HouseSceneGrid * grid = new HouseSceneGrid(this);

	grid->setGridNo(x, z);
	grid->setGridType(typeID);
	grid->setRoomID(roomID);

	mGridMap.insert(GridMap::value_type(GridNo(x,z), grid));

	return grid;
}
//-----------------------------------------------------------------------
HouseSceneGrid* HouseSceneFloor::getGrid( int x, int z ) 
{
	GridMap::iterator it = mGridMap.find(GridNo(x,z));
	if ( it == mGridMap.end() )
		return NULL;
	else
		return it->second;
}
//-----------------------------------------------------------------------
const HouseSceneGrid* HouseSceneFloor::getGrid( int x, int z ) const
{
	GridMap::const_iterator it = mGridMap.find(GridNo(x,z));
	if ( it == mGridMap.end() )
		return NULL;
	else
		return it->second;
}
//-----------------------------------------------------------------------
bool HouseSceneFloor::hasGrid( int x, int z ) const
{
	return mGridMap.find(GridNo(x,z)) != mGridMap.end();
}
//-----------------------------------------------------------------------
bool HouseSceneFloor::hasGrids() const
{
	return !mGridMap.empty();
}
//-----------------------------------------------------------------------
void HouseSceneFloor::destroyGrid( int x, int z )
{
	assert( hasGrid(x,z) );

	GridMap::iterator it = mGridMap.find(GridNo(x,z));
	delete it->second;
	mGridMap.erase(it);
}
//-----------------------------------------------------------------------
void HouseSceneFloor::setUpFloor(HouseSceneFloor* up)
{
	if ( !up )
	{
		if ( mUpFloor )
			mUpFloor->mDownFloor = NULL;
		mUpFloor = NULL;
		return;
	}

	assert( !mUpFloor );
	assert( !up->mDownFloor );
	mUpFloor = up;
	up->mDownFloor = this;
}
//-----------------------------------------------------------------------
void HouseSceneFloor::setDownFloor(HouseSceneFloor* down)
{
	if ( !down )
	{
		if ( mDownFloor )
			mDownFloor->mDownFloor = NULL;
		mDownFloor = NULL;
		return;
	}

	assert( !mDownFloor );
	assert( !down->mUpFloor );
	mDownFloor = down;
	down->mUpFloor = this;
}
//-----------------------------------------------------------------------
void HouseSceneFloor::filterByRoomID( int roomid )
{
	_showRoomIDPlane( true, true, roomid );
}
//-----------------------------------------------------------------------
void HouseSceneFloor::_showGridTypePlane( bool bShow )
{
	// 先删除
	_destroyManualObject( mGridTypePlane );
	mGridTypePlane = NULL;
	if ( !bShow )
		return;

	// 再建新的
	static int iCount = 0;
	++iCount;
	std::string name = "--FloorGridTypePlane--" + Ogre::StringConverter::toString( iCount );

	std::string matName = "SystemMat/Comm-1";

	mGridTypePlane = mSceneNode->getCreator()->createManualObject(name);

	mGridTypePlane->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	QuadBuilder qb( mGridTypePlane, 0.3 );
	for ( GridMap::iterator it = mGridMap.begin(); it != mGridMap.end(); ++it )
	{
		HouseSceneGrid * gr = it->second;

		Ogre::ColourValue col = gr->getHouseGridTypeRow()->ColorSign;
		col.a = 0.3;
		qb.drawQuad( gr->getGridX() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE, 
					gr->getGridZ() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE, 
					HOUSE_GRID_HALF_SIZE - 3,
					HOUSE_GRID_HALF_SIZE - 3,
					col ); 
	}

	mGridTypePlane->end();

	mGridTypePlane->setQueryFlags( 0 );
	mGridTypePlane->setVisible( bShow );

	mSceneNode->attachObject( mGridTypePlane );
}
//-----------------------------------------------------------------------
void HouseSceneFloor::_showWallPlane( bool bShow )
{
	// 先删除
	_destroyManualObject( mWallPlane );
	mWallPlane = NULL;
	if ( !bShow )
		return;

	// 再建新的
	static int iCount = 0;
	++iCount;
	std::string name = "--FloorGridWallPlane--" + Ogre::StringConverter::toString( iCount );

	std::string matName = "SystemMat/Comm-2";

	mWallPlane = mSceneNode->getCreator()->createManualObject(name);

	mWallPlane->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	QuadBuilder qb( mWallPlane, 0.8 );
	for ( GridMap::iterator it = mGridMap.begin(); it != mGridMap.end(); ++it )
	{
		HouseSceneGrid * gr = it->second;
		_drawWall( qb, gr );
	}

	mWallPlane->end();

	mWallPlane->setQueryFlags( 0 );
	mWallPlane->setVisible( bShow );

	mSceneNode->attachObject( mWallPlane );		
}
//-----------------------------------------------------------------------
void HouseSceneFloor::_showHoldGridPlane( bool bShow, bool bFitmentMode )
{
	// 先删除
	_destroyManualObject( mHoldGridPlane );
	mHoldGridPlane = NULL;
	if ( !bShow )
		return;
	
	// 再建新的
	static int iCount = 0;
	++iCount;
	std::string name = "--FloorHoldGridPlane--" + Ogre::StringConverter::toString( iCount );

	std::string matName = "SystemMat/Comm-1";

	mHoldGridPlane = mSceneNode->getCreator()->createManualObject(name);

	mHoldGridPlane->begin(matName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

	const Ogre::ColourValue col(1,1,1,0.6);
	const int dis = HOUSE_GRID_HALF_SIZE - 3;

	QuadBuilder qb( mHoldGridPlane, 0.1 );
	for ( GridMap::iterator it = mGridMap.begin(); it != mGridMap.end(); ++it )
	{
		HouseSceneGrid * gr = it->second;
		if ( bFitmentMode )
		{
			if ( !gr->getHouseGridTypeRow()->ShowGridFitment )	//家装模式中的网格显示
				continue;
		}
		const int x = gr->getGridX() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE;
		const int z = gr->getGridZ() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE;

		qb.drawQuad( x + dis, z, 3, HOUSE_GRID_HALF_SIZE, col ); 
		qb.drawQuad( x - dis, z, 3, HOUSE_GRID_HALF_SIZE, col ); 
		qb.drawQuad( x, z + dis, HOUSE_GRID_HALF_SIZE, 3, col ); 
		qb.drawQuad( x, z - dis, HOUSE_GRID_HALF_SIZE, 3, col ); 
	}

	mHoldGridPlane->end();

	mHoldGridPlane->setQueryFlags( 0 );
	mHoldGridPlane->setVisible( bShow );

	mSceneNode->attachObject( mHoldGridPlane );
}


//-----------------------------------------------------------------------
void HouseSceneFloor::insertRoomComponent(uint room, HouseSceneComponent* comp)
{
	assert(room);

	RoomComponentMap::iterator it = mRoomComponentMap.find(room);
	if(it == mRoomComponentMap.end())
	{
		mRoomComponentMap.insert(RoomComponentMap::value_type(room, RoomComponent()));
		it = mRoomComponentMap.find(room);
	}

	switch(comp->getHouseCompPropRow()->PropID)
	{
	case HCP_FLOOR:
		assert(!it->second.floor);
		it->second.floor = comp;
		break;

	case HCP_OUT_WALL:
	case HCP_IN_WALL:
		assert(std::find(it->second.wall.begin(), it->second.wall.end(), comp) == it->second.wall.end());
		it->second.wall.push_back(comp);
		break;

	case HCP_TCX:
		assert(std::find(it->second.tcx.begin(), it->second.tcx.end(), comp) == it->second.tcx.end());
		it->second.tcx.push_back(comp);
		break;

	case HCP_NONE:
	case HCP_STATIC:
	case HCP_ROOF:
		assert(std::find(it->second.sta.begin(), it->second.sta.end(), comp) == it->second.sta.end());
		it->second.sta.push_back(comp);
		break;

	default:
		if(std::find(it->second.comp.begin(), it->second.comp.end(), comp) == it->second.comp.end())
			it->second.comp.push_back(comp);
		break;
	}
}
//-----------------------------------------------------------------------
void HouseSceneFloor::removeRoomComponent(uint room, HouseSceneComponent* comp)
{
	assert(room);

	RoomComponentMap::iterator it = mRoomComponentMap.find(room);
	if(it == mRoomComponentMap.end())
	{
		EQ_EXCEPT("Can't find Zone '" + 
			Ogre::StringConverter::toString(room) + "'!", 
			"FloorComponentInfo::changeZoneWallMaterial");
		return;
	}

	switch(comp->getHouseCompPropRow()->PropID)
	{
	case HCP_FLOOR:
		assert(it->second.floor == comp);
		it->second.floor = NULL;
		break;

	case HCP_OUT_WALL:
	case HCP_IN_WALL:
		assert(std::find(it->second.wall.begin(), it->second.wall.end(), comp) != it->second.wall.end());
		std::remove(it->second.wall.begin(), it->second.wall.end(), comp);
		break;

	case HCP_TCX:
		assert(std::find(it->second.tcx.begin(), it->second.tcx.end(), comp) != it->second.tcx.end());
		std::remove(it->second.tcx.begin(), it->second.tcx.end(), comp);
		break;

	case HCP_NONE:
	case HCP_STATIC:
	case HCP_ROOF:
		assert(std::find(it->second.sta.begin(), it->second.sta.end(), comp) != it->second.sta.end());
		std::remove(it->second.sta.begin(), it->second.sta.end(), comp);
		break;

	default:
		assert(std::find(it->second.comp.begin(), it->second.comp.end(), comp) != it->second.comp.end());
		std::remove(it->second.comp.begin(), it->second.comp.end(), comp);
		break;
	}
}
//-----------------------------------------------------------------------
void HouseSceneFloor::changeRoomFloorMaterial(uint room, std::string& floormat)
{
	assert(room);

	RoomComponentMap::iterator it = mRoomComponentMap.find(room);
	if(it == mRoomComponentMap.end())
	{
		EQ_EXCEPT("Can't find Zone '" + 
			Ogre::StringConverter::toString(room) + "'!", 
			"HouseSceneFloor::changeZoneWallMaterial");
		return;
	}

	assert(it->second.floor);
	it->second.floor->getModel()->changeEntityMat(floormat);
}
//-----------------------------------------------------------------------
void HouseSceneFloor::changeRoomWallMaterial(uint room, std::string& wallmat)
{
	assert(room);

	RoomComponentMap::iterator it = mRoomComponentMap.find(room);
	if(it == mRoomComponentMap.end())
	{
		EQ_EXCEPT("Can't find Zone '" + 
			Ogre::StringConverter::toString(room) + "'!", 
			"HouseSceneFloor::changeZoneWallMaterial");
		return;
	}

	for(int i=0; i<(int)it->second.wall.size(); i++)
	{
		it->second.wall[i]->getModel()->changeEntityMat(wallmat);
	}
}
//-----------------------------------------------------------------------
void HouseSceneFloor::changeRoomTcxMaterial(uint room, std::string& tcxmat)
{
	assert(room);

	RoomComponentMap::iterator it = mRoomComponentMap.find(room);
	if(it == mRoomComponentMap.end())
	{
		EQ_EXCEPT("Can't find Zone '" + 
			Ogre::StringConverter::toString(room) + "'!", 
			"HouseSceneFloor::changeRoomTcxMaterial");
		return;
	}

	for(int i=0; i<(int)it->second.tcx.size(); i++)
	{
		it->second.tcx[i]->getModel()->changeEntityMat(tcxmat);
	}
}
//-----------------------------------------------------------------------
HouseSceneFloor::RoomComponent* HouseSceneFloor::getRoomComponent(uint room)
{
	RoomComponentMap::iterator it = mRoomComponentMap.find(room);
	if (mRoomComponentMap.end() == it)
	{
		return NULL;
	}
	return &it->second;
}
//-----------------------------------------------------------------------
std::string HouseSceneFloor::getRoomFloorMaterial(uint room)
{
	RoomComponent* roomComp = getRoomComponent(room);
	if (roomComp)
	{
		std::vector<std::string> matList = roomComp->floor->getModel()->getEntityMat();
		if (!matList.empty())
		{
			return matList[0];
		}
	}
	return "";
}
//-----------------------------------------------------------------------
std::string HouseSceneFloor::getRoomWallMaterial(uint room)
{
	RoomComponent* roomComp = getRoomComponent(room);
	if (roomComp && !roomComp->wall.empty())
	{
		std::vector<std::string> matList = roomComp->wall[0]->getModel()->getEntityMat();
		if (!matList.empty())
		{
			return matList[0];
		}
	}
	return "";
}
//-----------------------------------------------------------------------
std::string HouseSceneFloor::getRoomTcxMaterial(uint room)
{
	RoomComponent* roomComp = getRoomComponent(room);
	if (roomComp && !roomComp->tcx.empty())
	{
		std::vector<std::string> matList = roomComp->tcx[0]->getModel()->getEntityMat();
		if (!matList.empty())
		{
			return matList[0];
		}
	}
	return "";
}
//-----------------------------------------------------------------------
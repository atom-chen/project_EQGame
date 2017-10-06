#include "EQMainPCH.h"

#include "HouseSceneGrid.h"
#include "HouseScene.h"
#include "HSCManager.h"
#include "HouseSceneFloor.h"
#include "HouseSceneDef.h"
#include "CommFuncGcs.h"
#include "SystemGcs.h"
#include "HouseTable.h"

struct CompareComponentByHeight
{
public:
	bool operator () ( HouseSceneComponent * left, HouseSceneComponent * right ) const
	{
		return left->getBottom() < right->getBottom();
	}
};

//-----------------------------------------------------------------------
HouseSceneGrid::HouseSceneGrid(HouseSceneFloor* floor) 
	: mCreator(floor)
	, mSceneNode(NULL)
	, mRoomID(0)
	, mBlocked(false)
	, mIsFrameGrid(false)
	, mHouseGridTypeRow(NULL)
	, mGridX(0)
	, mGridZ(0)
{
	for ( int i = 0; i < WALL_FACE_COUNT; ++i )
		mWallFace[i] = false;

	mSceneNode = floor->getCreator()->getSceneNode()->createChildSceneNode();
}
//-----------------------------------------------------------------------
HouseSceneGrid::~HouseSceneGrid()
{
	// 
	//_exceptionIfHasComp();

	//
	for ( int i = 0; i < WALL_FACE_COUNT; ++i )
	{
		if ( mWallFace[i] )
			setWallFace(i, false);
	}

	mSceneNode->getParentSceneNode()->removeAndDestroyChild(mSceneNode->getName());
}
//-----------------------------------------------------------------------
void HouseSceneGrid::attachComponent( HouseSceneComponent * com )
{
	// 按高度从小到大排序加入
	ComponentList::iterator it = std::lower_bound( mHouseSceneComponentList.begin(), mHouseSceneComponentList.end(), com, CompareComponentByHeight() );
	mHouseSceneComponentList.insert( it, com );
}
//-----------------------------------------------------------------------
void HouseSceneGrid::detachComponent( HouseSceneComponent * com )
{	
	for ( ComponentList::iterator it = mHouseSceneComponentList.begin(); it != mHouseSceneComponentList.end(); ++it )
	{
		if ( *it == com )
		{
			mHouseSceneComponentList.erase( it );	
			break;
		}
	}
}
//-----------------------------------------------------------------------
bool HouseSceneGrid::isNeighborGrid( const HouseSceneGrid * grid, int& face ) const
{
	if ( mCreator != grid->mCreator )
		return false;

	if ( isFrameGrid() || grid->isFrameGrid() )
		return false;

	if ( mGridX+1 == grid->mGridX && mGridZ == grid->mGridZ )
	{
		face = 0;
		return true;
	}

	if ( mGridX-1 == grid->mGridX && mGridZ == grid->mGridZ )
	{
		face = 2;
		return true;
	}

	if ( mGridX == grid->mGridX && mGridZ+1 == grid->mGridZ )
	{
		face = 3;
		return true;
	}

	if ( mGridX == grid->mGridX && mGridZ-1 == grid->mGridZ )
	{
		face = 1;
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------
void HouseSceneGrid::frameGrid( bool b )
{
	_exceptionIfHasComp();

	mIsFrameGrid = b;
}
//-----------------------------------------------------------------------
void HouseSceneGrid::setGridNo( int x, int z )
{
	_exceptionIfHasComp();

	// 网格节点无效
	if ( mIsFrameGrid )
		return;

	mGridX = x;
	mGridZ = z;

	_updatePosition();
}
//-----------------------------------------------------------------------
void HouseSceneGrid::setGridType( ushort typeID )
{
	_exceptionIfHasComp();

	if ( mHouseGridTypeRow )
		EQ_EXCEPT("GridType can't change!", "HouseSceneGrid::setGridType");

	mHouseGridTypeRow = HouseTableManager::getSingleton().getHouseGridTypeRow(typeID);
	// 阻挡属性
	mBlocked = ( HOUSE_BLOCKED_GRID_TYPE == typeID );


	if ( !mHouseGridTypeRow )
	{
		EQ_EXCEPT("Can't find Grid Type id '" + Ogre::StringConverter::toString(typeID) + "'",
			"HouseSceneGrid::setGridType");
	}
}
//-----------------------------------------------------------------------
HouseSceneGrid* HouseSceneGrid::setWallFace( int face, bool wall )
{
	_exceptionIfHasComp();

	if ( mIsFrameGrid )
		EQ_EXCEPT("Can't be frame grid!", "HouseSceneGrid::setWallFace");

	int x,z;
	getFaceGridNo(face, x, z);
	int nf = negativeFace(face);

	HouseSceneGrid* grid = mCreator->getGrid(x, z);
	if ( !grid )
		EQ_EXCEPT("Can't find neighbor grid!", "HouseSceneGrid::setWallFace");

	assert( grid->mWallFace[nf] == mWallFace[face] );

	if ( mWallFace[face] != wall )
	{
		mWallFace[face] = wall;

		//
		grid->_exceptionIfHasComp(); 
		assert( !grid->mIsFrameGrid );
		grid->mWallFace[nf] = wall;
	}
	
	return grid;
}
//-----------------------------------------------------------------------
int HouseSceneGrid::negativeFace( int face )
{
	int nf = 0;
	switch( face )
	{
	case 0:
		nf = 2;
		break;
	case 1:
		nf = 3;
		break;
	case 2:
		nf = 0;
		break;
	case 3:
		nf = 1;
		break;
	}

	return nf;
}
//-----------------------------------------------------------------------
void HouseSceneGrid::getFaceGridNo( int face, int & x, int & z ) const
{
	x = mGridX;
	z = mGridZ;
	int nf = negativeFace(face);

	switch( face )
	{
	case 0:
		++x;
		break;
	case 1:
		--z;
		break;
	case 2:
		--x;
		break;
	case 3:
		++z;
		break;
	}
}
//-----------------------------------------------------------------------
void HouseSceneGrid::_exceptionIfHasComp()
{
	//maybe change
	if ( !mHouseSceneComponentList.empty() )
		EQ_EXCEPT( "Component is no empty!", "HouseSceneGrid::_exceptionIfHasComp" );

	//if ( !hasWallFace() )
	//	EQ_EXCEPT( "wallFace is no empty!", "HouseSceneGrid::_exceptionIfHasComp" );
}
//-----------------------------------------------------------------------
bool HouseSceneGrid::hasWallFace() const
{
	for ( int i = 0; i < WALL_FACE_COUNT; ++i )
	{
		if ( mWallFace[i] )
			return true;
	}
	return false;
}
//-----------------------------------------------------------------------
Ogre::SceneNode * HouseSceneGrid::getSceneNode()
{
	return mSceneNode;
}
//-----------------------------------------------------------------------
const Ogre::SceneNode * HouseSceneGrid::getSceneNode() const
{
	return mSceneNode;
}
//-----------------------------------------------------------------------
void HouseSceneGrid::_updatePosition()
{
	//更新节点坐标
	Ogre::Vector3 vec;
	int height = getCreator()->getFloorHeight();
	vec.y = (float)height;
	vec.x = HOUSE_GRID_SIZE * getGridX() + HOUSE_GRID_HALF_SIZE;
	vec.z = HOUSE_GRID_SIZE * getGridZ() + HOUSE_GRID_HALF_SIZE;

	if ( mSceneNode->getPosition() != vec )
		mSceneNode->setPosition(vec);
}
//-----------------------------------------------------------------------
void HouseSceneGrid::calcWatchMode( HouseWatchMode mode )
{
	const HouseSceneGrid::ComponentList & ls = getHouseSceneComponentList();
	HouseSceneGrid::ComponentList::const_iterator it;
	for ( it = ls.begin(); it != ls.end(); ++it )
	{
		// 物件可能被多次计算, 可以考虑一个物件是否是放在主位还是镜像位
		(*it)->calcWatchMode(mode);
	}
}
//-----------------------------------------------------------------------
void HouseSceneGrid::setBlocked( bool bVal)
{
	mBlocked = bVal;
}																		
//-----------------------------------------------------------------------
#include "EQMainPCH.h"

#include "HouseScene.h"
#include "SubHouseRes.h"
#include "HSCManager.h"
#include "HouseResManager.h"
#include "CommFuncGcs.h"
#include "CommFunc.h"
#include "SystemGcs.h"

const std::string HOUSE_SCENE_NODE_NAME = "__HouseSceneNode__";

//-----------------------------------------------------------------------
HouseScene::HouseScene(Ogre::Camera* cam) 
{
	mCamera = cam;
	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if ( mgr->hasSceneNode(HOUSE_SCENE_NODE_NAME) )
		mSceneNode = mgr->getSceneNode(HOUSE_SCENE_NODE_NAME);
	else
		mSceneNode = mgr->getRootSceneNode()->createChildSceneNode(HOUSE_SCENE_NODE_NAME);

	_initGraphics();

	mRootHouseFloor = NULL;
	mWalkViewFloor = NULL;
	mWatchMode = HW_MODE_1;
	mGridDisplayMode = GD_MODE_GAME;

	mMatPathBak = "";
	mHouseLoaded = false;
	mHouseResName = "";
}
//-----------------------------------------------------------------------
HouseScene::~HouseScene()
{
	//mComponentMap处理. 还必须根据摆放的层次不同排序
	{
		std::map<uint, HouseSceneComponent*> comps = mComponentMap;
		for ( std::map<uint, HouseSceneComponent*>::iterator it = comps.begin(); it != comps.end(); ++it )
		{
			HSCManager::getSingleton().destroyHouseSceneComponent(it->second);
		}
		assert( mComponentMap.empty() );
	}

	//floors
	{
		std::map<int, HouseSceneFloor*>::iterator it;
		for ( it = mFloors.begin(); it != mFloors.end(); ++it )
		{
			delete it->second;
		}
		mFloors.clear();
	}

	_destroyGraphics();

	mRootHouseFloor = NULL;
	mWalkViewFloor = NULL;


	// 删除自己的节点
	assert( mSceneNode->numChildren()==0 );
	//mSceneNode->getParentSceneNode()->removeAndDestroyChild(mSceneNode->getName());
}
//-----------------------------------------------------------------------
void HouseScene::buildHouse()
{
	if ( mHouseLoaded )
		EQ_EXCEPT( "House is loaded!", "HouseScene::buildHouse" );

	assert( !mHouseResName.empty() );

	//确保加载
	HouseResPtr house = HouseResManager::getSingleton().getByName(mHouseResName);
	house->load();

	_buildFloors(house);
	_buildComps(house);

	//
	mHouseLoaded = true;
}
//-----------------------------------------------------------------------
HouseSceneFloor* HouseScene::getFloorByNum(int n)
{
	std::map<int, HouseSceneFloor*>::iterator it = mFloors.find(n);
	if ( it == mFloors.end() )
		return NULL;
	else
		return it->second;
}
//-----------------------------------------------------------------------
void HouseScene::getXZ( int & x, int & z, const Ogre::Ray & ray ) 
{
	x = 0;
	z = 0;

	Ogre::Vector3 centerVec = getSceneNode()->_getDerivedPosition() + Ogre::Vector3( 0, (float)mWalkViewFloor->getFloorHeight(), 0 );
	Ogre::Plane pla( Ogre::Vector3::UNIT_Y, centerVec );
	std::pair<bool, float> rt = Ogre::Math::intersects(ray, pla);
	if ( rt.first )	// 如果有相交
	{
		Ogre::Vector3 worldCrossingPoint = ray.getPoint( rt.second );
		Ogre::Vector3 vec = worldCrossingPoint - centerVec;
		x = (int)floor(vec.x/HOUSE_GRID_SIZE);
		z = (int)floor(vec.z/HOUSE_GRID_SIZE);
	}
}
//-----------------------------------------------------------------------
bool HouseScene::rayGrid( const Ogre::Ray& ray, HouseSceneGrid *& gr, Ogre::Vector3 & worldCrossingPoint, bool allHouse )
{
	gr = NULL;
	worldCrossingPoint = Ogre::Vector3::ZERO;

	if ( !mRootHouseFloor || !mWalkViewFloor )
		return false;

	// 控制相机与 交叉点的距离
	const float dis = 10000000000.0f;

	// 因为我们的游戏只会从+y往-y发现pick, 所以这么写
	HouseSceneFloor * houseFloor;
	if ( allHouse )
	{
		HouseSceneFloor * tf = mRootHouseFloor;
		do 
		{
			houseFloor = tf;
			tf = tf->getUpFloor();
		} while (tf != NULL);
	}
	else
		houseFloor = mWalkViewFloor;

	for ( ; houseFloor != NULL; 
		houseFloor = houseFloor->getDownFloor() )
	{
		int height = houseFloor->getFloorHeight();
		Ogre::Vector3 centerVec  = getSceneNode()->_getDerivedPosition() + Ogre::Vector3(0,(float)height, 0);
		Ogre::Plane pla( Ogre::Vector3::UNIT_Y, centerVec );
		std::pair<bool, float> rt = Ogre::Math::intersects(ray, pla);

		if ( rt.first )	// 如果有相交
		{
			worldCrossingPoint = ray.getPoint( rt.second );
			Ogre::Vector3 vec = worldCrossingPoint - centerVec;
			int x = (int)floor(vec.x/HOUSE_GRID_SIZE);
			int z = (int)floor(vec.z/HOUSE_GRID_SIZE);

			if ( houseFloor->hasGrid(x,z) && dis > rt.second )
			{
				gr = houseFloor->getGrid(x,z);				
				return true;
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------
HouseSceneGrid* HouseScene::gridByPosition(const Ogre::Vector3& pos, float YBias, float NeYBias)
{
	HouseSceneGrid* ret = NULL;

	HouseSceneFloor* topFloor;
	HouseSceneFloor* tf = mRootHouseFloor;
	do 
	{
		topFloor = tf;
		tf = tf->getUpFloor();
	} while (tf != NULL);

	//
	for ( HouseSceneFloor* fl = topFloor; fl != NULL; fl = fl->getDownFloor() )
	{
		float height = (float)fl->getFloorHeight();
		if (pos.y < (height-NeYBias) || pos.y >= (height+YBias))
		{
			//不再范围内
			continue;
		}

		Ogre::Vector3 centerVec = getSceneNode()->_getDerivedPosition() + Ogre::Vector3( 0, height, 0 );
		Ogre::Vector3 vec = pos - centerVec;
		int x = (int)floor(vec.x/HOUSE_GRID_SIZE);
		int z = (int)floor(vec.z/HOUSE_GRID_SIZE);
		ret = getGrid(fl->getFloorNO(), x, z);

		break;
	}

	return ret;
}
//-----------------------------------------------------------------------
bool HouseScene::canConvertToResource(std::string& reason)
{
	reason = "OK!";

	if ( !mHouseLoaded )
	{
		reason = "房屋未被加载!";
		return false;
	}

	std::set<std::string> nameList;

	std::map<uint, HouseSceneComponent*>::iterator it;
	for ( it = mComponentMap.begin(); it != mComponentMap.end(); ++it )
	{
		HouseSceneComponent* c = it->second;
		if ( c->getResName().empty() )
		{
			reason = "房屋部件id '" + Ogre::StringConverter::toString(c->getUid()) + "' 没有资源名字!";
			return false;
		}

		if ( nameList.find(c->getResName()) != nameList.end() )
		{
			reason = "房屋部件id '" + Ogre::StringConverter::toString(c->getUid()) + "' 资源名重名!";
			return false;
		}

		nameList.insert(std::set<std::string>::value_type(c->getResName()));
	}

	//目前认为肯定有楼层
	assert( mRootHouseFloor );

	return true;
}
//-----------------------------------------------------------------------
HouseResPtr HouseScene::convertToResource()
{
	std::string str;
	if ( !canConvertToResource(str) )
	{
		EQ_EXCEPT( "Can't conbert to resource!", "HouseScene::convertToResource" );
	}

	const std::string resName = "-HouseSceneConvertHouseResource-";
	HouseResPtr ptr = HouseResManager::getSingleton().createManual(resName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, &g_NullManualResourceLoader );
	ptr->mAddMatFullName = mMatPathBak;

	//===================================
	ptr->mFloorRootNO = mRootHouseFloor->getFloorNO();

	HouseSceneFloor* flr = mRootHouseFloor;
	while(flr)
	{
		FloorHouseRes* fs = ptr->createFloorHouseRes(flr->getFloorNO());
		fs->mHeight = flr->getFloorHeight();

		for ( HouseSceneFloor::GridMap::iterator it = flr->mGridMap.begin(); it != flr->mGridMap.end(); ++it )
		{
			HouseSceneGrid* grid = it->second;

			FloorHouseRes::Grid gs;
			gs.mType = grid->getHouseGridTypeRow()->TypeID;
			grid->getGridNo(gs.mX, gs.mZ);
			gs.mRoom = grid->getRoomID();
			fs->mGrids.push_back(gs);

			//墙只取2面,可以避免重复
			for ( int i = 0; i <= 1; ++i )
			{
				if ( grid->getWallFace(i) )
				{
					FloorHouseRes::Wall ws;
					grid->getGridNo(ws.mX0, ws.mZ0);
					grid->getFaceGridNo(i, ws.mX1, ws.mZ1);
					fs->mWalls.push_back(ws);
				}
			}
		}

		//
		flr = flr->getUpFloor();
		if ( flr )
			fs->mUpFloorNO = flr->getFloorNO();
	}

	// 房屋部件
	for ( std::map<uint, HouseSceneComponent*>::iterator it = mComponentMap.begin(); it != mComponentMap.end(); ++it )
	{
		HouseSceneComponent* c = it->second;
		ComponenetHouseRes* cs = ptr->createComponenetHouseRes(c->getResName());

		c->writeRes(cs);
	}
	//===================================
	// 梯子，门的连接信息
	for ( PairGridList::iterator it = mPairGridList.begin(); it != mPairGridList.end(); ++it )
	{
		LinkGridRes lgr;
		lgr.mStartGrid.mFloorNo = it->first->getCreator()->getFloorNO();
		lgr.mStartGrid.mX =	it->first->getGridX();
		lgr.mStartGrid.mZ = it->first->getGridZ();

		lgr.mEndGrid.mFloorNo = it->second->getCreator()->getFloorNO();
		lgr.mEndGrid.mX = it->second->getGridX();
		lgr.mEndGrid.mZ = it->second->getGridZ();

		ptr->addLinkGrid( lgr );
	}

	// 确保加载
	ptr->load();
	/// 删除关联
	HouseResManager::getSingleton().remove(ptr->getName());
	return ptr;
}
//-----------------------------------------------------------------------
void HouseScene::_attachComponent(HouseSceneComponent* comp)
{
	if ( mComponentMap.find(comp->getUid()) != mComponentMap.end() )
		EQ_EXCEPT( "Comp is exist!", "HouseScene::_attachComponent" );

	mComponentMap[comp->getUid()] = comp;
}
//-----------------------------------------------------------------------
void HouseScene::_detachComponent(uint id)
{
	mComponentMap.erase(id);
}
//-----------------------------------------------------------------------
void HouseScene::_buildComps(HouseResPtr house)
{
	//
	//房屋部件
	HouseRes::ComponenetHouseResMap& cs = house->mComponenetHouseResMap;

	int maxLayer = 0;
	std::vector<std::list<ComponenetHouseRes*>> clv;

	//第一次扫描,扫描不同layer的物件
	for ( HouseRes::ComponenetHouseResMap::iterator it = cs.begin(); it != cs.end(); ++it )
	{
		ComponenetHouseRes* c = it->second;
		if ( c->mLayer >= 32 || c->mLayer < 0)
			EQ_EXCEPT("Invaid layer value!", "HouseScene::_buildComps");

		if ( clv.size() <= (size_t)c->mLayer )
			clv.resize(c->mLayer+1);

		clv[c->mLayer].push_back(c);
	}

	//在不同层级的列表添加物件
	for ( size_t i = 0; i < clv.size(); ++i )
	{
		for ( std::list<ComponenetHouseRes*>::iterator it = clv[i].begin(); it != clv[i].end(); ++it )
		{
			ComponenetHouseRes* c = *it;
			HouseSceneGrid* gr = NULL;

			if ( mFloors.find(c->mFlr) == mFloors.end() )
				EQ_EXCEPT("Invaid Comp flr!", "HouseScene::_buildComps");

			if ( c->mFrame )
				gr = mFloors[c->mFlr]->getRootGrid();
			else
			{
				gr = mFloors[c->mFlr]->getGrid(c->mX, c->mZ);
				if ( !gr )
					EQ_EXCEPT("Invaid Comp x z!", "HouseScene::_buildComps");
			}

			//创建物件
			HouseSceneComponent* hsc = HSCManager::getSingleton().createHouseSceneComponent();
			hsc->parseRes(c, house->mAddPath);
			hsc->load();

			//暂时的, 和上面的convertToResource中的layer为0一起处理
			hsc->lay( gr, c->mFace, c->mBottom );
		}
	}

	// 处理梯子(暂时用斜面表示)
	for ( HouseRes::LinkGridList::const_iterator it = house->getLinkGridList().begin();
		it != house->getLinkGridList().end();
		++it )
	{		
		HouseSceneGrid * aGrid = getGrid( it->mStartGrid.mFloorNo, it->mStartGrid.mX, it->mStartGrid.mZ );
		HouseSceneGrid * bGrid = getGrid( it->mEndGrid.mFloorNo, it->mEndGrid.mX, it->mEndGrid.mZ );

		addStair( aGrid, bGrid );
	}

	//资源分析结束
	//=========================
	//
	HouseSceneFloor* upWalkFloor = mRootHouseFloor;
	mWalkViewFloor = NULL;
	while(upWalkFloor)
	{
		mWalkViewFloor = upWalkFloor;
		upWalkFloor = upWalkFloor->getUpFloor();
	}

	setWatchMode( HW_MODE_2 );
	// 走到最低层
	while( downView() ) {}

}
//-----------------------------------------------------------------------
void HouseScene::_buildFloors(HouseResPtr house)
{
	mMatPathBak = house->mAddMatFullName;

	HouseRes::FloorHouseResMap& fs = house->mFloorHouseResMap;

	if ( fs.size() == 0 )
		EQ_EXCEPT( "House Resource no floor!", "HouseScene::_buildFloors" );


	//增加材质, 同时可以自动附加目录
	std::string matRes = mMatPathBak;
	std::string baseName;
	std::string ExtName;
	std::string pathName;
	Ogre::StringUtil::splitFullFilename(mMatPathBak, baseName, ExtName, pathName);
	if ( pathName.empty() && !ExistResource(mMatPathBak) ) //没有路径, 且资源不存在
	{
		std::string baseName1;
		std::string ExtName1;
		std::string pathName1;
		std::string resName = house->getName();
		Ogre::StringUtil::splitFullFilename(resName, baseName1, ExtName1, pathName1);
		if ( !pathName1.empty() )
		{
			std::string newMat = pathName1 + mMatPathBak;
			if ( ExistResource(newMat) )
				matRes = newMat;
		}
	}

	if ( !matRes.empty() && ExistResource(matRes) )
	{
		ParseOgreScript(matRes);
	}

	//第一扫描获取楼层数据
	for ( HouseRes::FloorHouseResMap::iterator it = fs.begin(); it != fs.end(); ++it )
	{
		FloorHouseRes* f = it->second;

		HouseSceneFloor* flr = new HouseSceneFloor(this);
		//目前0打算留给花园使用
		if ( f->mNO <= 0 && f->mNO >=256 )
			EQ_EXCEPT("Invaid Floor NO!", "HouseScene::_buildFloors");

		mFloors[f->mNO] = flr;

		//楼层属性
		flr->mFloorNO = f->mNO;
		flr->modifyFloorHeight(f->mHeight);

		//楼层的网格
		for ( std::vector<FloorHouseRes::Grid>::iterator git = f->mGrids.begin(); git != f->mGrids.end(); ++git )
		{
			FloorHouseRes::Grid& grid = (*git);

			flr->createGrid(grid.mX, grid.mZ, grid.mType, grid.mRoom);
		}

		//楼层的墙
		for ( std::vector<FloorHouseRes::Wall>::iterator wit = f->mWalls.begin(); wit != f->mWalls.end(); ++wit )
		{
			FloorHouseRes::Wall& wall = *wit;
			HouseSceneGrid* ga = flr->getGrid(wit->mX0, wit->mZ0);
			HouseSceneGrid* gb = flr->getGrid(wit->mX1, wit->mZ1);
			if ( !ga || !gb )
				EQ_EXCEPT("Invaid Walls Data!", "HouseScene::_buildFloors");

			int face;
			if ( !ga->isNeighborGrid(gb, face) )
				EQ_EXCEPT("Invaid Walls Data!", "HouseScene::_buildFloors");

			ga->setWallFace(face, true);
		}
	}

	//第二扫描获取楼层楼上楼下关系
	for ( HouseRes::FloorHouseResMap::iterator it = fs.begin(); it != fs.end(); ++it )
	{
		FloorHouseRes* f = it->second;

		if ( f->mUpFloorNO >= 256 )
			continue;
		if ( mFloors.find(f->mUpFloorNO) == mFloors.end() )
			EQ_EXCEPT("Invaid upFloorNO!", "HouseScene::_buildFloors");

		mFloors[f->mNO]->setUpFloor(mFloors[f->mUpFloorNO]);
	}

	//rootNo
	if ( mFloors.find(house->mFloorRootNO) == mFloors.end() )
		EQ_EXCEPT("Invaid upFloorNO!", "HouseScene::_buildFloors");
	mRootHouseFloor = mFloors[house->mFloorRootNO];

}

//-----------------------------------------------------------------------
HouseSceneGrid * HouseScene::getGrid( int flrNO, int x, int z )
{
	HouseSceneFloor * flr = this->getFloorByNum( flrNO );
	if ( flr )
	{
		return flr->getGrid( x, z );
	}

	return NULL;
}

//-----------------------------------------------------------------------
void HouseScene::addStair( HouseSceneGrid * a, HouseSceneGrid * b )
{
	/// 梯子先这样处理
	if ( mPairGridList.find( PairGrid( a, b ) ) != mPairGridList.end() )
		return;

	mPairGridList.insert( PairGrid( a, b ) );

	addStairGraphics( a, b );
}

//-----------------------------------------------------------------------
void HouseScene::removeStair( HouseSceneGrid * a, HouseSceneGrid * b )
{
	/// 梯子先这样处理
	if( mPairGridList.find( PairGrid( a, b ) ) == mPairGridList.end() )
		return;

	mPairGridList.erase( PairGrid( a, b ) );

	removeStairGraphics( a, b );
}
//-----------------------------------------------------------------------
void HouseScene::update(float delta)
{
	if ( mHouseLoaded )
	{
		// 更新墙体遮挡
		if ( mWalkViewFloor && mWatchMode == HW_MODE_2 )
		{
			mWalkViewFloor->calcWatchMode( mWatchMode );
		}

		// 更新辅助平面高度
		_updateAssistPlaneHeight();
	}
}
//-----------------------------------------------------------------------
bool HouseScene::upView()
{
	HouseSceneFloor * flr = mWalkViewFloor;

	if ( !flr )
		return false;

	if ( flr->getUpFloor() )
	{
		if ( mWatchMode == HW_MODE_4 )
			flr->calcWatchMode( HW_MODE_4 );
		else
			flr->calcWatchMode( HW_MODE_3 );

		if ( mGridDisplayMode != GD_MODE_FITMENT )
			flr->calcGridDisplayMode(GD_MODE_HIDE);

		//
		flr = flr->getUpFloor();
		setWalkViewFloor(flr);

		flr->calcWatchMode( mWatchMode );
		flr->calcGridDisplayMode( mGridDisplayMode );
		return true;
	}
	else
		return false;

}
//-----------------------------------------------------------------------
bool HouseScene::downView()
{
	HouseSceneFloor * flr = mWalkViewFloor;

	if ( !flr )
		return false;

	if ( flr->getDownFloor() )
	{
		flr->calcWatchMode(HW_MODE_HIDE);
		flr->calcGridDisplayMode(GD_MODE_HIDE);

		//
		flr = flr->getDownFloor();
		setWalkViewFloor(flr);

		flr->calcWatchMode(mWatchMode);
		flr->calcGridDisplayMode(mGridDisplayMode);
		return true;
	}
	else
	{
		return false;
	}
}
//-----------------------------------------------------------------------
void HouseScene::changeWatchMode(HouseWatchMode mode)
{
	if ( mode == mWatchMode )
		return;

	bool walk = false;
	if ( mWatchMode == HW_MODE_4 || mode == HW_MODE_4 )
		walk = true;

	mWatchMode = mode;

	//
	if ( mWalkViewFloor && mRootHouseFloor )
	{
		HouseSceneFloor* floor = mRootHouseFloor;
		
		while( walk && floor != mWalkViewFloor )
		{
			if ( mWatchMode == HW_MODE_4 )
				floor->calcWatchMode(HW_MODE_4);
			else
				floor->calcWatchMode(HW_MODE_3);

			floor = floor->getUpFloor();
		}

		mWalkViewFloor->calcWatchMode(mWatchMode);
	}
}
//-----------------------------------------------------------------------
HouseWatchMode HouseScene::getWatchModeByFloorNO(int flrNO) const
{
	std::map<int, HouseSceneFloor *>::const_iterator it = mFloors.find(flrNO);
	if ( it == mFloors.end() )
		EQ_EXCEPT( "Floor No found!", "HouseScene::getWatchModeByFloorNO" );

	//HouseSceneFloor* flr = it->second;
	if ( flrNO == mWalkViewFloor->getFloorNO() )
		return mWatchMode;
	else if ( flrNO > mWalkViewFloor->getFloorNO() )	//在上面
		return HW_MODE_HIDE;
	else												//在下面
	{
		if ( mWatchMode == HW_MODE_4 )
			return HW_MODE_4;
		else
			return HW_MODE_3;
	}
}
//-----------------------------------------------------------------------
void HouseScene::changeGridDisplayMode(GridDisplayMode mode)
{
	if ( mode == mGridDisplayMode )
		return;

	mGridDisplayMode = mode;

	if ( mWalkViewFloor && mRootHouseFloor )
	{
		HouseSceneFloor * floor = mRootHouseFloor;
		while (floor != mWalkViewFloor) 
		{
			if ( GD_MODE_FITMENT == mode )
				floor->calcGridDisplayMode(mode);
			else
				floor->calcGridDisplayMode(GD_MODE_HIDE);


			floor = floor->getUpFloor();
		}
		floor->calcGridDisplayMode(mode);
	}

	// 控制梯子的显示或隐藏
	showStairGraphics( mGridDisplayMode == GD_MODE_CONNECTEDNESS );
}
//-----------------------------------------------------------------------
void HouseScene::_destroyGraphics()
{
	_destroyManualObject( mFloorAssistPlane );
	for ( PairGridObjectList::iterator it = mPairGridObjectList.begin();
		it != mPairGridObjectList.end();
		++it )
	{
		_destroyManualObject( it->second );
	}
}
//-----------------------------------------------------------------------
void HouseScene::_initGraphics()
{
	mFloorAssistPlane = NULL;
	mFloorAssistGridNum = 0;
	mFloorAssistPlaneHeight = 0;
}
//-----------------------------------------------------------------------
void HouseScene::addStairGraphics( HouseSceneGrid * a, HouseSceneGrid * b )
{
	PairGrid gr( a, b );
	PairGridObjectList::iterator it = mPairGridObjectList.find( gr );
	assert( it == mPairGridObjectList.end() );

	// 根据斜度 简单算出是哪面 (不是X方向，就是Z方向)
	bool isXDir = true;
	if ( std::abs( a->getGridZ() - b->getGridZ() ) > std::abs( a->getGridX() - b->getGridX() ) )
		isXDir = false;

	Ogre::Vector3 v1, v2, v3, v4;

	if ( isXDir )
	{
		v1 = Ogre::Vector3( a->getGridX() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE, 
							a->getCreator()->getFloorHeight(), 
							a->getGridZ() * HOUSE_GRID_SIZE );

		v2 = v1 + Ogre::Vector3( 0, 0, HOUSE_GRID_SIZE );

		v3 = Ogre::Vector3( b->getGridX() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE, 
							b->getCreator()->getFloorHeight(), 
							b->getGridZ() * HOUSE_GRID_SIZE );

		v4 = v3 + Ogre::Vector3( 0, 0, HOUSE_GRID_SIZE );
	}
	else
	{
		v1 = Ogre::Vector3( a->getGridX() * HOUSE_GRID_SIZE, 
							a->getCreator()->getFloorHeight(), 
							a->getGridZ() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE );

		v2 = v1 + Ogre::Vector3( HOUSE_GRID_SIZE, 0, 0 );

		v3 = Ogre::Vector3( b->getGridX() * HOUSE_GRID_SIZE, 
							b->getCreator()->getFloorHeight(), 
							b->getGridZ() * HOUSE_GRID_SIZE + HOUSE_GRID_HALF_SIZE );

		v4 = v3 + Ogre::Vector3( HOUSE_GRID_SIZE, 0, 0 );
	}
	
	Ogre::ManualObject * obj = ::CreateManualQuad( mCamera->getSceneManager(), v1, v2, v4, v3, Ogre::ColourValue( 1, 1, 0, 0.5 ) );
	Ogre::SceneNode * node = mCamera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	node->attachObject( obj );

	mPairGridObjectList[gr] = obj;

	obj->setVisible( getGridDisplayMode() == GD_MODE_CONNECTEDNESS );
}
//-----------------------------------------------------------------------
void HouseScene::removeStairGraphics( HouseSceneGrid * a, HouseSceneGrid * b )
{
	PairGrid gr( a, b );
	PairGridObjectList::iterator it = mPairGridObjectList.find( gr );
	assert( it != mPairGridObjectList.end() );

	_destroyManualObject( it->second );

	mPairGridObjectList.erase( it );
}
//-----------------------------------------------------------------------
void HouseScene::showStairGraphics( bool bVal )
{
	for ( PairGridObjectList::iterator it = mPairGridObjectList.begin();
		it != mPairGridObjectList.end();
		++it )
	{
		it->second->setVisible( bVal );
	}
}
//-----------------------------------------------------------------------
void HouseScene::_destroyManualObject( Ogre::ManualObject * obj )
{
	if ( !obj )
		return;

	Ogre::SceneNode * node = obj->getParentSceneNode();
	node->detachObject( obj );
	mCamera->getSceneManager()->destroyManualObject( obj );
	mCamera->getSceneManager()->destroySceneNode( node );
}

//-----------------------------------------------------------------------
void HouseScene::setFloorAssistGridNum( int n )
{
	if ( mFloorAssistGridNum == n )
		return;

	mFloorAssistGridNum = n;

	bool bVisible = mFloorAssistPlane && mFloorAssistPlane->getVisible();
	_destroyManualObject( mFloorAssistPlane );
	_createAssistPlane( bVisible );
}
//-----------------------------------------------------------------------
void HouseScene::_updateAssistPlaneHeight()
{
	if ( !mWalkViewFloor )
		return;

	const int floorHeight = mWalkViewFloor->getFloorHeight();
	if ( floorHeight == mFloorAssistPlaneHeight )
		return;

	if ( !mFloorAssistPlane )
		return;

	mFloorAssistPlaneHeight = floorHeight;

	Ogre::SceneNode * node = mFloorAssistPlane->getParentSceneNode();

	//调整节点
	Ogre::Vector3 v = mSceneNode->_getDerivedPosition() + Ogre::Vector3(0,floorHeight, 0);
	node->setPosition( v );
}
//-----------------------------------------------------------------------
void HouseScene::showAssistPlane( bool bShow )
{
	if ( mFloorAssistPlane )
		this->mFloorAssistPlane->setVisible( bShow );
}
//-----------------------------------------------------------------------
void HouseScene::_createAssistPlane( bool bVisible )
{
	Ogre::SceneManager * mgr = mCamera->getSceneManager();

	Ogre::SceneNode * node = mSceneNode->createChildSceneNode();

	const std::string name = "--FloorAaasitPlane--ByHouseEditor--";

	mFloorAssistPlane = CreateManualGrid( mgr, name,
										HOUSE_GRID_SIZE, mFloorAssistGridNum, 0,
										Ogre::ColourValue(0.5, 0.5, 0.5), Ogre::ColourValue::White );

	node->attachObject( mFloorAssistPlane );

	mFloorAssistPlane->setVisible( bVisible );
	mFloorAssistPlaneHeight = 0;
}
//-----------------------------------------------------------------------
void HouseScene::turnAllLight(bool on)
{
	for(std::map<uint, HouseSceneComponent *>::iterator it=mComponentMap.begin(); it!=mComponentMap.end(); it++)
		on ? it->second->turnOnLight() : it->second->turnOffLight();
}
//-----------------------------------------------------------------------
#include "EQMainPCH.h"

#include "HouseSceneComponent.h"
#include "SubHouseRes.h"
#include "CommFuncGcs.h"
#include "ModelManagerGcs.h"
#include "SystemGcs.h"
#include "GraphicsTable.h"
#include "HouseSceneGrid.h"
#include "CommFunc.h"
#include "SceneManager.h"
#include "ZoneLightMgr.h"
#include "HouseScene.h"
#include "HSCSheetShadow.h"

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
HouseSceneComponent::HouseSceneComponent(uint uid) 
	: mUid(uid)
	, mLoaded(false)
	, mModel(NULL)
	, mPos( Ogre::Vector3::ZERO )
	, mRot( Ogre::Quaternion::IDENTITY )
	, mBottom( 0 )
	, mFlr( -1 )
	, mRoom( 0 )
	, mSheetShadow(NULL)
	, mSheetShadowNode(NULL)
	//, mAttachGrid(NULL)
{
	mProxy.setObject(this);

	_initProperty();
}
//-----------------------------------------------------------------------
HouseSceneComponent::~HouseSceneComponent()
{
	if ( mLoaded )
	{
		if(mFlr>0)
		{
			HouseSceneFloor* flr = EQSceneManager::getSingleton().getHouseScene()->getFloorByNum(mFlr);
			if(flr && mRoom!=0)
				flr->removeRoomComponent(mRoom, this);
		}

		if(mLights.size() > 0)
		{
			for(int i=0; i<(int)mLights.size(); i++)
				ZoneLightMgr::getSingleton().destroyZoneLight(mLights[i]);
		}

		unLay();

		if ( mModel )
		{
			ModelManagerGcs::getSingleton().destroyModel(mModel);
			mModel = NULL;
		}
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::parseRes(ComponenetHouseRes* res, const std::string& addPath)
{
	assert( !mLoaded );

	mAddPath = "";

	mResName = res->mName;
	mIsFromFrame = res->mFrame;
	mFlr = res->mFlr;

	if ( mIsFromFrame )
	{
		mAddPath = addPath;

		mFrmMesh = res->mFrmMesh;

		mFrmPos = res->mFrmPos;
		mFrmRot = res->mFrmRot;
		mFrmScale = res->mFrmScale;

		mHouseCompPropRow = HouseTableManager::getSingleton().getHouseCompPropRow(res->mFrmCompType);
		if ( !mHouseCompPropRow )
		{
			EQ_EXCEPT( "Can't find HouseCompProp Type '" + Ogre::StringConverter::toString(res->mFrmCompType) + "'",
				"HouseSceneComponent::parseRes" );
		}

		// 框架物件只能在一个房间
		mHoldZone.push_back(room2zone(res->mRoom!=0, res->mFlr, res->mRoom));
		mRoom = res->mRoom;
	}
	else
	{
		mBottom = res->mBottom;
		mHouseCompRow = HouseTableManager::getSingleton().getHouseCompRow(res->mCompId);
		if ( !mHouseCompRow )
			EQ_EXCEPT( FormatStr("Can't find HouseComp.xml data '%d'", res->mCompId), "HouseSceneComponent::parseRes" );

		mHouseCompPropRow = HouseTableManager::getSingleton().getHouseCompPropRow(mHouseCompRow->PropID);
		if ( !mHouseCompPropRow )
			EQ_EXCEPT( FormatStr("Can't find HouseCompProp.xml data '%d'", res->mCompId), "HouseSceneComponent::parseRes" );
	}

	mFace = res->mFace;
}
//-----------------------------------------------------------------------
const std::string& HouseSceneComponent::getClassName()
{
	static std::string name = "HouseSceneComponent";
	return name;
}
//-----------------------------------------------------------------------
void HouseSceneComponent::parseRes( uint compId )
{
	assert( !mLoaded );
	mHouseCompRow = HouseTableManager::getSingleton().getHouseCompRow(compId);
	if ( !mHouseCompRow )
		EQ_EXCEPT( FormatStr("Can't find HouseComp.xml data '%d'", compId), "HouseSceneComponent::parseRes" );

	mHouseCompPropRow = HouseTableManager::getSingleton().getHouseCompPropRow(mHouseCompRow->PropID);
	if ( !mHouseCompPropRow )
		EQ_EXCEPT( FormatStr("Can't find HouseCompProp.xml data '%d'", compId), "HouseSceneComponent::parseRes" );
}
//-----------------------------------------------------------------------
void HouseSceneComponent::parseRes( const HouseCompRow * compRow, const HouseCompPropRow * propRow )
{
	assert( !mLoaded );

	mIsFromFrame = false;
	mHouseCompRow = compRow;
	mHouseCompPropRow = propRow;

	if ( !mHouseCompRow )
		EQ_EXCEPT( "mHouseCompRow不能为空", "HouseSceneComponent::parseRes" );

	if ( !mHouseCompPropRow )
		EQ_EXCEPT( "mHouseCompPropRow不能为空", "HouseSceneComponent::parseRes" );
}
//-----------------------------------------------------------------------
void HouseSceneComponent::writeRes(ComponenetHouseRes* res)
{
	assert( res->mName == mResName );
	res->mFrame = mIsFromFrame;

	if ( mIsFromFrame )
	{
		res->mFrmCompType = mHouseCompPropRow->PropID;
		res->mFrmMesh = mFrmMesh;
		res->mFrmPos = mFrmPos;
		res->mFrmRot = mFrmRot;
		res->mFrmScale = mFrmScale;
	}
	else
	{
		assert(mHouseCompRow);
		res->mCompId = mHouseCompRow->ID;
	}

	res->mBottom = mBottom;
	res->mFace = getFace();
	res->mLayer = 0;
	res->mRoom = mRoom;

	//
	if ( isLayed() )
	{
		HouseSceneGrid* grid = getAttachGrid();
		res->mFlr = grid->getCreator()->getFloorNO();
		grid->getGridNo(res->mX, res->mZ);
	}

}
//-----------------------------------------------------------------------
void HouseSceneComponent::load()
{
	if ( mLoaded )
		return;

	//模型
	if ( mIsFromFrame )
	{
		assert(!mFrmMesh.empty());

		SimpleCreateModelRule rule(mAddPath + mFrmMesh);

		mModel = ModelManagerGcs::getSingleton().createModel(&rule);
		mModel->setPickObjectProxy(&mProxy);

		Ogre::SceneNode* node = mModel->getMainNode();
		node->setPosition(mFrmPos);
		node->setOrientation(mFrmRot);
		node->setScale(mFrmScale);
	}
	else
	{
		const ModelDisplayRow * r = GraphicsTableManager::getSingleton().getModelDisplayRow( mHouseCompRow->DisID );
		SimpleCreateModelRule rule( r->mesh, r->mat, r->particle );

		mModel = ModelManagerGcs::getSingleton().createModel( &rule );
		mModel->setPickObjectProxy(&mProxy);

		Ogre::SceneNode* node = mModel->getMainNode();

		node->setPosition( mHouseCompRow->Pos );
		node->setOrientation( mHouseCompRow->Rot );
		node->setScale( mHouseCompRow->Scale );	
	}

	//占据网格
	{
		HouseCompPropRow::GridPosArray::const_iterator it =  mHouseCompPropRow->HoldGrid.begin();
		for ( ; it != mHouseCompPropRow->HoldGrid.end(); ++it )
			mHoldBox.merge(it->x, it->y);
		assert( !mHoldBox.mIsNull );

		mHoldGrids.resize(mHoldBox.volume(), NULL);
	}

	mLoaded = true;
}
//-----------------------------------------------------------------------
const Ogre::Vector3 & HouseSceneComponent::getPosition() const
{
	return mPos;
}
//-----------------------------------------------------------------------
const Ogre::Quaternion & HouseSceneComponent::getOrientation() const
{
	return mRot;
}
//-----------------------------------------------------------------------
void HouseSceneComponent::setPosition( const Ogre::Vector3& pos )
{
	mPos = pos;
	_updatePosAndRot();
}
//-----------------------------------------------------------------------
void HouseSceneComponent::setOrientation( const Ogre::Quaternion& rot )
{
	mRot = rot;
	_updatePosAndRot();
}
//-----------------------------------------------------------------------
void HouseSceneComponent::setParentNode( Ogre::SceneNode* parentNode )
{
	assert(mModel);
	mModel->setParentNode(parentNode);
	if ( mSheetShadowNode )
	{
		if ( mSheetShadowNode->getParentSceneNode() )
			mSheetShadowNode->getParentSceneNode()->removeChild(mSheetShadowNode);
		parentNode->addChild(mSheetShadowNode);
	}
	setPosition(Ogre::Vector3::ZERO);
	setOrientation(Ogre::Quaternion::IDENTITY);
}
//-----------------------------------------------------------------------
void HouseSceneComponent::calcWatchMode( HouseWatchMode mode )
{
	if ( mHouseCompPropRow->BeWatchMode + mode > CW_MODE_LIMIT )
	{
		setVisible(true);
	}
	else if ( mHouseCompPropRow->BeWatchMode + mode == CW_MODE_LIMIT )
	{
		// 如果是外墙，计算隐藏
		if ( mHouseCompPropRow->BeWatchMode == CW_MODE_OUTWALL )
		{
			// 面对隐藏
			if ( this->getAttachGrid() )
			{
				Ogre::Camera * cam = this->getAttachGrid()->getCreator()->getCreator()->getCamera();
				Ogre::Vector3 nor = mModel->getMainNode()->_getDerivedOrientation() * Ogre::Vector3::UNIT_X;
				Ogre::Vector3 vec = mModel->getMainNode()->_getDerivedPosition() - cam->getPosition();
				if ( vec.dotProduct(nor) < 0.0001 ) 
					setVisible(false);
				else
					setVisible(true);
			}
		}
		else
		{
			setVisible(true);
		}
	}
	else
	{
		setVisible(false);
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::convertGrids( HouseCompPropRow::GridPosArray & arr, int face, int gridX, int gridZ )
{
	for ( HouseCompPropRow::GridPosArray::iterator it = arr.begin();
		it != arr.end();
		++it )
	{
		// rotateQuarter 其实可以放到commFunc
		Int2AABBBox::rotateQuarter( it->x, it->y, face, true );
		it->x += gridX;
		it->y += gridZ;
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::_updatePosAndRot()
{
	// 设置模型的坐标
	Ogre::Vector3 vec;
	Ogre::Quaternion rot;

	if ( mIsFromFrame )
	{
		vec = mFrmPos;
		rot = mFrmRot;
	}
	else
	{
		vec = mHouseCompRow->Pos;
		rot = mHouseCompRow->Rot;
	}

	assert(mModel);
	Ogre::SceneNode* node = mModel->getMainNode();

	node->setPosition( mPos + mRot * vec );
	node->setOrientation( mRot * rot );

	if ( mSheetShadowNode )
	{
		mSheetShadowNode->setPosition( mPos + Ogre::Vector3(0, 0.5, 1) );
		mSheetShadowNode->setOrientation( mRot );
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::lay( HouseSceneGrid * grid, int face, int height )
{	
	mBottom = height;
	mFace = face;

	// 框架所在房间在parseRes时计算的
	if(!mIsFromFrame)
		mRoom = grid->getRoomID();

	// 非编辑器的话，需要通过格子信息来获取楼层号
	if(mFlr == -1)
		mFlr = grid->getCreator()->getFloorNO();

	// 占据网格
	std::vector<const HouseSceneGrid*> holdGrids = _calcHoldGrids( grid, face );
	mHoldGrids.clear();
	for ( std::vector<const HouseSceneGrid*>::iterator it = holdGrids.begin(); it != holdGrids.end(); ++it )
	{
		HouseSceneGrid * pGrid = const_cast<HouseSceneGrid *>(*it);
		pGrid->attachComponent(this);
		mHoldGrids.push_back(pGrid);
	}

	// 如果是最底下的一个
	if ( this == *grid->getHouseSceneComponentList().begin() )
	{
		// 设置阻挡属性
		HouseCompPropRow::GridPosArray stopGrids = this->getHouseCompPropRow()->StopGrid;
		convertGrids( stopGrids, face, grid->getGridX(), grid->getGridZ() );

		for ( HouseCompPropRow::GridPosArray::iterator it = stopGrids.begin(); it != stopGrids.end(); ++it )
		{
			HouseSceneGrid * gr = grid->getCreator()->getGrid( it->x, it->y );
			assert( gr );
			gr->setBlocked( true );
		}
	}

	//加面片阴影
	//贴地, 非框架, 非可以替换物件, 非光源
	if ( mHouseCompPropRow->OffsetHeight == 0 && mBottom == 0 && !isFromFrame() 
		&& !mHouseCompPropRow->CanRep && mHouseCompPropRow->LightID == 0 )
	{
		createSheetShadow();
	}

	setParentNode( grid->getSceneNode() );

	setOrientation( Ogre::Quaternion( Ogre::Radian(Ogre::Degree(90 * mFace) ), Ogre::Vector3::UNIT_Y) );
	if ( getHouseCompPropRow()->OffsetHeight == 0 )
		setPosition( Ogre::Vector3( 0, mBottom, 0 ) );
	else
		setPosition( Ogre::Vector3::ZERO );	//如果有逻辑高度的则不受mBottom的影响

	EQSceneManager::getSingleton().getHouseScene()->_attachComponent(this);

	// 框架物件在load时就已经计算好了，这里只计算非框架物件
	if(!mIsFromFrame)
	{
		// 计算物件所属房间，一般只会在一个房间，门和窗户会跨两个房间
		mHoldZone.clear();
		for(int i=0; i<(int)mHoldGrids.size(); i++)
		{
			uint zone = room2zone(mHoldGrids[i]->getHouseGridTypeRow()->InDoor, 
									mHoldGrids[i]->getCreator()->getFloorNO(), mHoldGrids[i]->getRoomID());
			if(std::find(mHoldZone.begin(), mHoldZone.end(), zone) == mHoldZone.end())
				mHoldZone.push_back(zone);
		}
	}

	// 创建监听
	if(mHoldZone.size() > 0)
		mModel->setLightListener(mHoldZone);

	// 加载携带光源
	if(!mIsFromFrame && mHouseCompPropRow->LightID!=0)
	{
		bool created = mLights.size()>0;

		bool forward_indoor;
		bool createPass = _checkPassLights(forward_indoor);

		if(!created)
		{
			const LightRow* l = GraphicsTableManager::getSingleton().getLightRow(mHouseCompPropRow->LightID);
			mLights = ZoneLightMgr::getSingleton().createZoneLights(l, createPass);
		}

		//改成创建完后再通知
		std::vector<std::pair<int, ZoneLight*>> changeZone;
		for(int i=0; i<(int)mLights.size(); i++)
		{
			if(mLights[i]->getZoneLightType() == ZoneLight::ZLT_ACTIVE)
			{
				if(!created)
				{
					// 主动光是直接就创建好的
					Ogre::SceneNode* lnode = mModel->getMainNode()->createChildSceneNode();
					mLights[i]->setNode(lnode);
				}

				// 这个要放后执行
				uint zone = room2zone(getAttachGrid()->getHouseGridTypeRow()->InDoor, 
										getAttachGrid()->getCreator()->getFloorNO(), getAttachGrid()->getRoomID());

				changeZone.push_back(std::make_pair(zone, mLights[i]));
			}
			else if(mLights[i]->getZoneLightType() == ZoneLight::ZLT_PASS)
			{
				// 这里是硬代码，只适用物件占据两格房间的情况
				int pierceZone;
				int holdZone;
				for(int j=0; j<(int)mHoldZone.size(); j++)
				{
					// 这里一般一格是所在房间、一格是影响的房间
					if(mLights[i]->getPassLightIndoor() == (mHoldZone[j]!=0))
					{
						holdZone = mHoldZone[j];

						if(!created)
						{
							// 穿透光是产生时才创建，所以要保存绑定的节点
							Ogre::SceneNode* lnode = mModel->getMainNode()->createChildSceneNode();
							mLights[i]->setNode(lnode);
						}
					}
					else
					{
						pierceZone = mHoldZone[j];
					}
				}

				// 保存住影响此穿透光的区域(即窗户另一面的格子所处区域)
				mLights[i]->setForwardIndoor(forward_indoor);
				mLights[i]->setPierceZoneID(pierceZone);
				
				changeZone.push_back(std::make_pair(holdZone, mLights[i]));
			}
		}
		for(uint i=0; i<changeZone.size(); i++)
			ZoneLightMgr::getSingleton().changeLightToZone(changeZone[i].first, changeZone[i].second);

		// lay完后要通知场景管理器刷新下，以修正摆放灯光有时会无法及时刷新灯光列表的bug
		if(mLights.size() > 0)
			SystemGcs::getSingleton().getSceneManager()->_notifyLightsDirty();
	}

	//粒子
	if(mHouseCompRow && mHouseCompRow->Particle.size()!=0)
	{
		for(uint i=0; i<mHouseCompRow->Particle.size(); i++)
		{
			if(mHouseCompRow->Particle[i])
				mModel->addParticle(mHouseCompRow->Particle[i]);
		}
	}

	//加入楼层部件管理
	if(mFlr>0)
	{
		HouseSceneFloor* flr = EQSceneManager::getSingleton().getHouseScene()->getFloorByNum(mFlr);
		if(flr && mRoom!=0)
			flr->insertRoomComponent(mRoom, this);
	}

	//重新计算使用格
	mUseGrids.clear();

	//刷新可见熟悉
	if ( EQSceneManager::getSingleton().getHouseScene()->getWalkViewFloor() )
		calcWatchMode(EQSceneManager::getSingleton().getHouseScene()->getWatchModeByFloorNO(mFlr));

}
//-----------------------------------------------------------------------
bool HouseSceneComponent::_checkPassLights(bool& forward_indoor)
{
	// 必须是靠墙物件
	if(!isLayWallObject())
		return false;

	// 占据网格必须是2*N
	if(mHoldGrids.size()%2 != 0)
		return false;

	// x轴正方向和反方向的第一格必须一个在室内、一个在室外(目前所有窗x轴只占两格)
	// 这里只进行(0,0)和(-1,0)或(1,0)的判断
	// 并保存正方向indoor值，即如果是(0,0)(1,0)保存(1,0)的indoor值，如果是(-1,0)(0,0)保存(0,0)的indoor值
	if(mHoldBox.mXmin<0 && (int)mHoldGrids.size()>mHoldBox.linearIndex(0,0))
	{
		forward_indoor = mHoldGrids[mHoldBox.linearIndex(0,0)]->getHouseGridTypeRow()->InDoor;
		if((mHoldGrids[mHoldBox.linearIndex(-1,0)]->getHouseGridTypeRow()->InDoor == forward_indoor))
			return false;
	}
	else if(mHoldBox.mXmax>1 && (int)mHoldGrids.size()>mHoldBox.linearIndex(1,0))
	{
		forward_indoor = mHoldGrids[mHoldBox.linearIndex(1,0)]->getHouseGridTypeRow()->InDoor;
		if((forward_indoor == mHoldGrids[mHoldBox.linearIndex(0,0)]->getHouseGridTypeRow()->InDoor))
			return false;
	}

	return true;
}
//-----------------------------------------------------------------------
void HouseSceneComponent::take()
{
	assert( HRV_OK == HSCLayLogic::canTake(this) );

	unLay();

	calcWatchMode(HW_MODE_3);
}
//-----------------------------------------------------------------------
void HouseSceneComponent::setVisible(bool vi)
{
	if ( mModel )
		mModel->setVisible(vi);

	if ( mSheetShadow )
		mSheetShadow->setVisible(vi);

	for(uint i=0; i<mLights.size(); i++)
	{
		mLights[i]->setMute(!vi);
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::_removeFromScene()
{
	unLay();
}
//-----------------------------------------------------------------------
void HouseSceneComponent::unLay()
{
	if ( !this->isLayed() )
		return;

	destroySheetShadow();

	HouseSceneGrid * grid = getAttachGrid();
	// 如果是最底下的一个
	if ( this == *grid->getHouseSceneComponentList().begin() )
	{
		// 设置阻挡属性
		HouseCompPropRow::GridPosArray stopGrids = this->getHouseCompPropRow()->StopGrid;
		convertGrids( stopGrids, this->getFace(), grid->getGridX(), grid->getGridZ() );

		for ( HouseCompPropRow::GridPosArray::iterator it = stopGrids.begin(); it != stopGrids.end(); ++it )
		{
			HouseSceneGrid * gr = grid->getCreator()->getGrid( it->x, it->y );
			assert( gr );
			gr->setBlocked( false );
		}
	}

	setParentNode(NULL);

	for ( std::vector<HouseSceneGrid*>::iterator it = mHoldGrids.begin(); it != mHoldGrids.end(); ++it )
	{
		(*it)->detachComponent(this);
		(*it) = NULL;
	}

	//通知
	EQSceneManager::getSingleton().getHouseScene()->_detachComponent(mUid);
}
//-----------------------------------------------------------------------
void HouseSceneComponent::_initProperty()
{
	//
	//mResName
	mIsFromFrame = false;
	mHouseCompPropRow = NULL;
	mFace = 0;


	//框架物件的特有属性集
	//mFrmMesh
	mFrmPos = Ogre::Vector3::ZERO;
	//mFrmRot
	mFrmScale = Ogre::Vector3::UNIT_SCALE;


	//房屋部件表的特有属性集
	mHouseCompRow = NULL;	
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
int HouseSceneComponent::getTop() const
{
	return getBottom() + mHouseCompPropRow->SelfHeight;
}
//-----------------------------------------------------------------------
int HouseSceneComponent::getBottom() const
{
	if ( mHouseCompPropRow->OffsetHeight != 0 )
		return this->mHouseCompPropRow->OffsetHeight;

	return mBottom;
}
//-----------------------------------------------------------------------
bool HouseSceneComponent::isDoorOrWin() const
{
	return this->mHouseCompPropRow->CanRep 
		&& (this->mHouseCompPropRow->HoldType != 0);
}
//-----------------------------------------------------------------------
bool HouseSceneComponent::isLayWallObject() const
{
	return (this->mHouseCompPropRow->HoldType != 0);
}
//-----------------------------------------------------------------------
// 取得所有格子对象
std::vector<const HouseSceneGrid *> HouseSceneComponent::_calcHoldGrids( const HouseSceneGrid * attachGrid, int face ) const
{	
	std::vector<const HouseSceneGrid *> gs;
	const HouseSceneFloor * flr = attachGrid->getCreator();
	if ( this->isFromFrame() )
	{
		gs.resize( 1, flr->getRootGrid() );
	}
	else
	{
		gs.resize(this->mHoldBox.volume(), NULL);
		for ( int x = this->mHoldBox.mXmin; x <= this->mHoldBox.mXmax; ++x )
		{
			for ( int z = this->mHoldBox.mYmin; z <= this->mHoldBox.mYmax; ++z )
			{
				int X(x), Z(z);
				Int2AABBBox::rotateQuarter(X, Z, face, true);
				X += attachGrid->getGridX();
				Z += attachGrid->getGridZ();
				const HouseSceneGrid * gr = flr->getGrid(X, Z);

				gs[this->mHoldBox.linearIndex(x,z)] = gr;
			}
		}
	}
	return gs;
}
//-----------------------------------------------------------------------
std::set<const HouseSceneGrid*>& HouseSceneComponent::calcUseGrids(Ogre::uchar face, uint range)
{
	// 计算过了就不再计算了
	if(mUseGrids.size() > 0)
		return mUseGrids;
	
	// 算出最大x、z
	int xmax=0,xmin=0,zmax=0,zmin=0;
	int x,z;
	for(int i=0; i<(int)mHoldGrids.size(); i++)
	{
		x = mHoldGrids[i]->getGridX();
		z = mHoldGrids[i]->getGridZ();
		if(i == 0)
		{
			xmax = xmin = x;
			zmax = zmin = z;
		}
		else
		{
			xmax = x>xmax ? x : xmax;
			xmin = x<xmin ? x : xmin;
			zmax = z>zmax ? z : zmax;
			zmin = z<zmin ? z : zmin;
		}
	}

	int aface[4];
	if((face & 0x1) > 0) aface[0]=1; else aface[0]=0;
	if((face & 0x2) > 0) aface[1]=1; else aface[1]=0;
	if((face & 0x4) > 0) aface[2]=1; else aface[2]=0;
	if((face & 0x8) > 0) aface[3]=1; else aface[3]=0;

	HouseSceneGrid* grid;
	bool cw,acw;
	for(int n=0; n<4; n++)
	{
		// 不能使用的面跳过
		if(!aface[n])
			continue;

		// 只有和邻方向同时有效，才加入拐角格
		cw = aface[ (n+1)%4 ] == 1;
		acw = aface[ (n-1+4)%4 ] == 1;

		// 计算使用格子
		HouseSceneFloor* flr = EQSceneManager::getSingleton().getHouseScene()->getFloorByNum(mFlr);
		assert(flr);

		for(int i=0; i<(int)mHoldGrids.size(); i++)
		{
			x = mHoldGrids[i]->getGridX();
			z = mHoldGrids[i]->getGridZ();
			switch( (n+mFace)%4 )
			{
			case 0:
				{
					// 朝x的只取xmax的就行
					if(x != xmax) continue;
					// x+1格必定有效
					grid = flr->getGrid(x+1, z);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmin)的要多取(xmax+1,zmin-1)那格
					grid = flr->getGrid(x+1, z-1);
					if(cw && z == zmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmax)的要多取(xmax+1,zmax+1)那格
					grid = flr->getGrid(x+1, z+1);
					if(acw && z == zmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			case 1:
				{
					// 朝-z的只取zmin的就行
					if(z != zmin) continue;
					// z-1格必定有效
					grid = flr->getGrid(x, z-1);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmin)的要多取(xmin-1,zmin-1)那格
					grid = flr->getGrid(x-1, z-1);
					if(cw && x == xmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmin)的要多取(xmax+1,zmin-1)那格
					grid = flr->getGrid(x+1, z-1);
					if(acw && x == xmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			case 2:
				{
					// 朝-x的只取xmin的就行
					if(x != xmin) continue;
					// x-1格必定有效
					grid = flr->getGrid(x-1, z);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmax)的要多取(xmin-1,zmax+1)那格
					grid = flr->getGrid(x-1, z+1);
					if(cw && z == zmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmin)的要多取(xmin-1,zmin-1)那格
					grid = flr->getGrid(x-1, z-1);
					if(acw && z == zmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			case 3:
				{
					// 朝z的只取zmax的就行
					if(z != zmax) continue;
					// z+1格必定有效
					grid = flr->getGrid(x, z+1);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmax)的要多取(xmax+1,zmax+1)那格
					grid = flr->getGrid(x+1, z+1);
					if(cw && z == zmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmax)的要多取(xin-1,zmax+1)那格
					grid = flr->getGrid(x-1, z+1);
					if(acw && z == zmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			default:
				assert(0);
			}
		}
	}

	return mUseGrids;
}
//-----------------------------------------------------------------------
std::vector<int>& HouseSceneComponent::calcInteractMorph(int pos, int flag, int user_count)
{
	assert(pos < user_count);

	if(mInteractMorph.size() != user_count)
	{
		mInteractMorph.clear();
		for(int i=0; i<user_count; i++)
			mInteractMorph.push_back(0);
	}

	mInteractMorph[pos] = flag;

	return mInteractMorph;
}
//-----------------------------------------------------------------------
void HouseSceneComponent::createSheetShadow()
{
	if ( mSheetShadow )
		return;
	assert(mSheetShadowNode == NULL);

	mSheetShadow = HSCSheetShadow::create(mHoldBox);
	mSheetShadowNode = SystemGcs::getSingleton().getSceneManager()->createSceneNode();
	mSheetShadowNode->attachObject(mSheetShadow);
	Ogre::SceneNode* node = mModel->getMainNode()->getParentSceneNode();
	if ( node )
		node->addChild(mSheetShadowNode);
}
//-----------------------------------------------------------------------
void HouseSceneComponent::destroySheetShadow()
{
	if ( mSheetShadow )
	{
		mSheetShadowNode->detachObject(mSheetShadow);
		if( mSheetShadowNode->getParentSceneNode() )
			mSheetShadowNode->getParentSceneNode()->removeAndDestroyChild(mSheetShadowNode->getName());
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mSheetShadow);
		mSheetShadow = NULL;
		mSheetShadowNode = NULL;
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::turnOnLight()
{
	if(mLights.size() > 0)
	{
		mModel->setLightMat();
		for(uint i=0; i<mLights.size(); i++)
			mLights[i]->turnOn();
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::turnOffLight()
{
	if(mLights.size() > 0)
	{
		mModel->clearLightMat();
		for(uint i=0; i<mLights.size(); i++)
			mLights[i]->turnOff();
	}
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
HouseSceneComponent * HSCLayLogic::findCanReplaceComponent( const HouseSceneComponent * pThis, const HouseSceneGrid * grid ) 
{
	if ( !pThis || !grid )
		return NULL;

	for ( HouseSceneGrid::ComponentList::const_iterator it = grid->getHouseSceneComponentList().begin();
		it != grid->getHouseSceneComponentList().end();
		++it )
	{
		if(*it == pThis)
			continue;

		if ( (*it)->getAttachGrid() == grid )
		{
			if ( canReplaceComponent( pThis, *it ) )
				return *it;
		}
	}

	return NULL;
}
//-----------------------------------------------------------------------
bool HSCLayLogic::canReplaceComponent( const HouseSceneComponent * pThis, const HouseSceneComponent * comp ) 
{
	// 此为游戏逻辑
	// 暂时先这样判断可替换与否

	return pThis->getHouseCompPropRow()->CanRep 
		&& (pThis->getHouseCompPropRow()->PropID == comp->getHouseCompPropRow()->PropID)
		//&& (pThis->getHouseCompRow()->ID != comp->getHouseCompRow()->ID)
		;	
}
//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
/** 在一个grid上，寻找物品摆放位置
	@ 返回值 -- 第一个可以用来摆放其上的物件
*/
//-----------------------------------------------------------------------
std::pair<bool, const HouseSceneComponent *> HSCLayLogic::getFirstCanLayComponent( const HouseSceneComponent * pThis, 
																						  const HouseSceneGrid * grid ) 
{
	assert( grid );

	typedef std::pair<bool, const HouseSceneComponent *> ResultType;

	// 判断格子上是否可放置this。(有些物件只能放在室内格子，有些物件只能放在室外格子)
	if ( !BitContains( grid->getHouseGridTypeRow()->AcceptLay, pThis->getHouseCompPropRow()->LayType ) )
	{
		return ResultType( false, NULL );
	}

	// 自身高度
	const int selfHeight = pThis->getHouseCompPropRow()->SelfHeight;

	const HouseSceneComponent * compDown = NULL;
	// 摆放底部值
	int hBottom = 0;

	const HouseSceneComponent * compUp = NULL;
	// 摆放顶部值
	int hTop = 0;
	
	HouseSceneGrid::ComponentList::const_iterator it = grid->getHouseSceneComponentList().begin();
	for ( ;	it != grid->getHouseSceneComponentList().end(); ++it )
	{
		//过滤占据不同格子空间的物件
		int layType = pThis->getHouseCompPropRow()->HoldType;
		int iteType = (*it)->getHouseCompPropRow()->HoldType;
		if(layType!=2 && iteType!=2 && layType!=iteType)
			continue;

		compUp = *it;
		hTop = compUp->getBottom();
		
		// 如果不是悬挂物件
		if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
		{
			// 如果碰到第一个悬挂物件，停止查找
			if ( compUp->getHouseCompPropRow()->OffsetHeight != 0 )
				break;
				
			if ( hBottom + selfHeight <= hTop )
			{
				return ResultType( true, compDown );
			}
		}
		else
		{
			if ( hBottom > pThis->getBottom() )
				return ResultType( false, NULL );
			else
			{
				if ( pThis->getTop() <= hTop )
				{
					return ResultType( true, compDown );
				}
			}
		}

		compDown = compUp;		
		hBottom = compDown->getTop();
	}

	// 如果是最后一个，则将顶部设为层高
	if ( it == grid->getHouseSceneComponentList().end() )
	{
		hTop = grid->getCreator()->getHeight();
		if ( hTop <= 0 )
			hTop = HOUSE_DEF_FLOOR_HEGHT;
	}
	
	// 如果不是悬挂物件
	if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
	{
		if ( hBottom + selfHeight <= hTop )
		{
			return ResultType( true, compDown );
		}
	}
	else
	{
		if ( hBottom <= pThis->getBottom() && ( pThis->getTop() <= hTop ) )
		{
			return ResultType( true, compDown );
		}
	}

	return ResultType( false, NULL );
}
//-----------------------------------------------------------------------
/** 自动靠墙
*/
HouseReturnValue HSCLayLogic::canAutoLayWall( const HouseSceneComponent * pThis, const HouseSceneGrid * grid, int & face, int & height ) 
{
	HouseReturnValue ret;
	for ( int i = 0; i < WALL_FACE_COUNT; ++i, ++face )
	{
		face = face % WALL_FACE_COUNT;
		ret = canLay( pThis, grid, face, height );
		if ( ret == HRV_OK )
			return ret;
	}
	return HRV_NO_AUTO_LAYWALL;
}
//-----------------------------------------------------------------------
HouseReturnValue HSCLayLogic::canLay( const HouseSceneComponent * pThis, const HouseSceneGrid * grid, int face, int & height ) 
{
	height = 0;

	if ( pThis->isLayed() )
		return HRV_LAYED;

	assert( grid );
	const HouseSceneFloor * flr = grid->getCreator();
	if ( pThis->isFromFrame() )		
	{
		return HRV_OK;
	}

	// 摆放高度默认为偏移高度
	height = pThis->getHouseCompPropRow()->OffsetHeight;

	// 取得当前this->HoldGrid, 转换成世界GridsArray
	HouseCompPropRow::GridPosArray holdGrid = pThis->getHouseCompPropRow()->HoldGrid;
	HouseSceneComponent::convertGrids( holdGrid, face, grid->getGridX(), grid->getGridZ() );

	// 根据this->SelfHeight + offsetHeight, 算出该Grid的最适合放置的comp
	const int selfHeight = pThis->getHouseCompPropRow()->SelfHeight;
	std::pair<bool, const HouseSceneComponent *> ret = getFirstCanLayComponent( pThis, grid );
	if ( !ret.first )
	{
		return HRV_NO_LAY_SPACE;
	}

	const HouseSceneComponent * comp = ret.second;

	if ( comp )
	{
		// 如果不是悬挂物件
		if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
		{
			// 是否可放置
			if ( !BitContains( comp->getHouseCompPropRow()->AcceptLay, pThis->getHouseCompPropRow()->LayType ) )
			{
				return HRV_NO_MATCH_LAYTYPE;
			}

			// 取出comp->LayGrid, 转换成世界GridsArray;
			HouseCompPropRow::GridPosArray layGrid = comp->getHouseCompPropRow()->LayGrid;

			const HouseSceneGrid * grid0 = comp->getAttachGrid();

			int x, z;
			grid0->getGridNo( x, z );
			HouseSceneComponent::convertGrids( layGrid, comp->getFace(), x, z );

			// 保证LayGrid 包含HoldGrid
			for ( HouseCompPropRow::GridPosArray::const_iterator it = holdGrid.begin();
				it != holdGrid.end();
				++it )
			{
				if ( std::find( layGrid.begin(), layGrid.end(), *it ) == layGrid.end() )
					return HRV_NO_GRID_LAY;
			}		
		}
	}

	// 取得所有格子对象
	std::vector<const HouseSceneGrid *> gs = pThis->_calcHoldGrids( grid, face );
	
	// 如果holdGrid 的每个格子，在comp高度上都有空间可以容下this
	for ( uint i = 0; i < gs.size(); ++i )
	{
		const HouseSceneGrid * p = gs[i];
		if ( p == grid )
			continue;

		if ( !p )
			return HRV_NO_GRID_LAY;

		std::pair<bool, const HouseSceneComponent *> r = getFirstCanLayComponent( pThis, p );
		
		if( !r.first )
			return HRV_NO_LAY_SPACE;

		// 如果不是悬挂物件
		if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
		{
			// 只能放在同一个物件上面
			if ( r.second != comp )
				return HRV_NO_LAY_SPACE;
		}
	}	

	//网格内有没有墙体阻挡(靠墙物件，允许内部有墙体)
	for ( int x = pThis->mHoldBox.mXmin; x <= pThis->mHoldBox.mXmax; ++x )
	{
		for ( int z = pThis->mHoldBox.mYmin; z <= pThis->mHoldBox.mYmax; ++z )
		{
			const HouseSceneGrid * p = gs[pThis->mHoldBox.linearIndex(x,z)];
			if ( x != pThis->mHoldBox.mXmax )
			{
				// 如果是靠墙物件，且为靠墙格子，不用检查此面墙
				if ( pThis->isLayWallObject() && pThis->mHoldBox.mXmin == x )
				{
				}
				else
				{
					if ( p->getWallFace(face) )
						return HRV_WALL_STOP;
				}
			}

			if ( z != pThis->mHoldBox.mYmin )
			{
				if ( p->getWallFace( (face+1)%4 ) )
					return HRV_WALL_STOP;
			}
		}
	}

	// 如果是靠墙物件
	if ( pThis->isLayWallObject() )
	{
		// 取靠墙格子
		HouseCompPropRow::GridPosArray backGrids;
		for ( HouseCompPropRow::GridPosArray::const_iterator it = pThis->getHouseCompPropRow()->HoldGrid.begin();
			it != pThis->getHouseCompPropRow()->HoldGrid.end();
			++it )
		{
			if ( it->x == 0 )
				backGrids.push_back( *it );
		}

		int x = 0;
		int z = 0;
		grid->getGridNo( x, z );
		HouseSceneComponent::convertGrids( backGrids, face, x, z );

		// 判断是否靠墙
		const int newFace = ( 2 + face ) % 4; 
		for ( HouseCompPropRow::GridPosArray::const_iterator it = backGrids.begin();
			it != backGrids.end();
			++it )
		{
			const HouseSceneGrid * p = flr->getGrid( it->x, it->y );
			assert ( p );

			if ( !p->getWallFace( newFace ) )
			{
				return HRV_NEAR_WALL;
			}
		}
	}

	// 如果不是悬挂物件
	if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
	{
		// 摆放高度
		if ( comp )
			height = comp->getTop();
	}

	return HRV_OK;
}
//-----------------------------------------------------------------------
HouseReturnValue HSCLayLogic::canTake( const HouseSceneComponent * pThis ) 
{
	if ( !pThis->isLayed() )
		return HRV_NO_LAYED;

	if ( pThis->mIsFromFrame )
		return HRV_NOTAKE_FRAME;

	// 如果是悬挂物件直接返回ok
	if ( pThis->getHouseCompPropRow()->OffsetHeight != 0 )
	{
		return HRV_OK;
	}

	// 如果上面有东西,则不能拾取
	const HouseSceneGrid * gridAttach = pThis->getAttachGrid();
	// 找出所有LayGrid, 对每个格子检查上面有没有物品摆放
	HouseCompPropRow::GridPosArray layGrids = pThis->getHouseCompPropRow()->LayGrid;

	HouseSceneComponent::convertGrids( layGrids, pThis->getFace(), gridAttach->getGridX(), gridAttach->getGridZ() );

	for ( HouseCompPropRow::GridPosArray::iterator it = layGrids.begin(); it != layGrids.end(); ++it )
	{
		const HouseSceneGrid * grid = gridAttach->getCreator()->getGrid( it->x, it->y );
		assert( grid );
		if ( grid )
		{
			HouseSceneGrid::ComponentList::const_iterator it;
			it = std::find( grid->getHouseSceneComponentList().begin(), grid->getHouseSceneComponentList().end(), pThis );
			assert( it != grid->getHouseSceneComponentList().end() );
			++it;

			if ( it != grid->getHouseSceneComponentList().end() )
			{
				if ( (*it)->getHouseCompPropRow()->OffsetHeight == 0 )
					return HRV_HAS_COMP_LAY;
			}		
		}
	}

	return HRV_OK;
}





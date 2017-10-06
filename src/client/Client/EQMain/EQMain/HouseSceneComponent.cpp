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

		// ������ֻ����һ������
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
		EQ_EXCEPT( "mHouseCompRow����Ϊ��", "HouseSceneComponent::parseRes" );

	if ( !mHouseCompPropRow )
		EQ_EXCEPT( "mHouseCompPropRow����Ϊ��", "HouseSceneComponent::parseRes" );
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

	//ģ��
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

	//ռ������
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
		// �������ǽ����������
		if ( mHouseCompPropRow->BeWatchMode == CW_MODE_OUTWALL )
		{
			// �������
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
		// rotateQuarter ��ʵ���Էŵ�commFunc
		Int2AABBBox::rotateQuarter( it->x, it->y, face, true );
		it->x += gridX;
		it->y += gridZ;
	}
}
//-----------------------------------------------------------------------
void HouseSceneComponent::_updatePosAndRot()
{
	// ����ģ�͵�����
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

	// ������ڷ�����parseResʱ�����
	if(!mIsFromFrame)
		mRoom = grid->getRoomID();

	// �Ǳ༭���Ļ�����Ҫͨ��������Ϣ����ȡ¥���
	if(mFlr == -1)
		mFlr = grid->getCreator()->getFloorNO();

	// ռ������
	std::vector<const HouseSceneGrid*> holdGrids = _calcHoldGrids( grid, face );
	mHoldGrids.clear();
	for ( std::vector<const HouseSceneGrid*>::iterator it = holdGrids.begin(); it != holdGrids.end(); ++it )
	{
		HouseSceneGrid * pGrid = const_cast<HouseSceneGrid *>(*it);
		pGrid->attachComponent(this);
		mHoldGrids.push_back(pGrid);
	}

	// ���������µ�һ��
	if ( this == *grid->getHouseSceneComponentList().begin() )
	{
		// �����赲����
		HouseCompPropRow::GridPosArray stopGrids = this->getHouseCompPropRow()->StopGrid;
		convertGrids( stopGrids, face, grid->getGridX(), grid->getGridZ() );

		for ( HouseCompPropRow::GridPosArray::iterator it = stopGrids.begin(); it != stopGrids.end(); ++it )
		{
			HouseSceneGrid * gr = grid->getCreator()->getGrid( it->x, it->y );
			assert( gr );
			gr->setBlocked( true );
		}
	}

	//����Ƭ��Ӱ
	//����, �ǿ��, �ǿ����滻���, �ǹ�Դ
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
		setPosition( Ogre::Vector3::ZERO );	//������߼��߶ȵ�����mBottom��Ӱ��

	EQSceneManager::getSingleton().getHouseScene()->_attachComponent(this);

	// ��������loadʱ���Ѿ�������ˣ�����ֻ����ǿ�����
	if(!mIsFromFrame)
	{
		// ��������������䣬һ��ֻ����һ�����䣬�źʹ��������������
		mHoldZone.clear();
		for(int i=0; i<(int)mHoldGrids.size(); i++)
		{
			uint zone = room2zone(mHoldGrids[i]->getHouseGridTypeRow()->InDoor, 
									mHoldGrids[i]->getCreator()->getFloorNO(), mHoldGrids[i]->getRoomID());
			if(std::find(mHoldZone.begin(), mHoldZone.end(), zone) == mHoldZone.end())
				mHoldZone.push_back(zone);
		}
	}

	// ��������
	if(mHoldZone.size() > 0)
		mModel->setLightListener(mHoldZone);

	// ����Я����Դ
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

		//�ĳɴ��������֪ͨ
		std::vector<std::pair<int, ZoneLight*>> changeZone;
		for(int i=0; i<(int)mLights.size(); i++)
		{
			if(mLights[i]->getZoneLightType() == ZoneLight::ZLT_ACTIVE)
			{
				if(!created)
				{
					// ��������ֱ�Ӿʹ����õ�
					Ogre::SceneNode* lnode = mModel->getMainNode()->createChildSceneNode();
					mLights[i]->setNode(lnode);
				}

				// ���Ҫ�ź�ִ��
				uint zone = room2zone(getAttachGrid()->getHouseGridTypeRow()->InDoor, 
										getAttachGrid()->getCreator()->getFloorNO(), getAttachGrid()->getRoomID());

				changeZone.push_back(std::make_pair(zone, mLights[i]));
			}
			else if(mLights[i]->getZoneLightType() == ZoneLight::ZLT_PASS)
			{
				// ������Ӳ���룬ֻ�������ռ�����񷿼�����
				int pierceZone;
				int holdZone;
				for(int j=0; j<(int)mHoldZone.size(); j++)
				{
					// ����һ��һ�������ڷ��䡢һ����Ӱ��ķ���
					if(mLights[i]->getPassLightIndoor() == (mHoldZone[j]!=0))
					{
						holdZone = mHoldZone[j];

						if(!created)
						{
							// ��͸���ǲ���ʱ�Ŵ���������Ҫ����󶨵Ľڵ�
							Ogre::SceneNode* lnode = mModel->getMainNode()->createChildSceneNode();
							mLights[i]->setNode(lnode);
						}
					}
					else
					{
						pierceZone = mHoldZone[j];
					}
				}

				// ����סӰ��˴�͸�������(��������һ��ĸ�����������)
				mLights[i]->setForwardIndoor(forward_indoor);
				mLights[i]->setPierceZoneID(pierceZone);
				
				changeZone.push_back(std::make_pair(holdZone, mLights[i]));
			}
		}
		for(uint i=0; i<changeZone.size(); i++)
			ZoneLightMgr::getSingleton().changeLightToZone(changeZone[i].first, changeZone[i].second);

		// lay���Ҫ֪ͨ����������ˢ���£��������ڷŵƹ���ʱ���޷���ʱˢ�µƹ��б��bug
		if(mLights.size() > 0)
			SystemGcs::getSingleton().getSceneManager()->_notifyLightsDirty();
	}

	//����
	if(mHouseCompRow && mHouseCompRow->Particle.size()!=0)
	{
		for(uint i=0; i<mHouseCompRow->Particle.size(); i++)
		{
			if(mHouseCompRow->Particle[i])
				mModel->addParticle(mHouseCompRow->Particle[i]);
		}
	}

	//����¥�㲿������
	if(mFlr>0)
	{
		HouseSceneFloor* flr = EQSceneManager::getSingleton().getHouseScene()->getFloorByNum(mFlr);
		if(flr && mRoom!=0)
			flr->insertRoomComponent(mRoom, this);
	}

	//���¼���ʹ�ø�
	mUseGrids.clear();

	//ˢ�¿ɼ���Ϥ
	if ( EQSceneManager::getSingleton().getHouseScene()->getWalkViewFloor() )
		calcWatchMode(EQSceneManager::getSingleton().getHouseScene()->getWatchModeByFloorNO(mFlr));

}
//-----------------------------------------------------------------------
bool HouseSceneComponent::_checkPassLights(bool& forward_indoor)
{
	// �����ǿ�ǽ���
	if(!isLayWallObject())
		return false;

	// ռ�����������2*N
	if(mHoldGrids.size()%2 != 0)
		return false;

	// x��������ͷ�����ĵ�һ�����һ�������ڡ�һ��������(Ŀǰ���д�x��ֻռ����)
	// ����ֻ����(0,0)��(-1,0)��(1,0)���ж�
	// ������������indoorֵ���������(0,0)(1,0)����(1,0)��indoorֵ�������(-1,0)(0,0)����(0,0)��indoorֵ
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
	// ���������µ�һ��
	if ( this == *grid->getHouseSceneComponentList().begin() )
	{
		// �����赲����
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

	//֪ͨ
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


	//���������������Լ�
	//mFrmMesh
	mFrmPos = Ogre::Vector3::ZERO;
	//mFrmRot
	mFrmScale = Ogre::Vector3::UNIT_SCALE;


	//���ݲ�������������Լ�
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
// ȡ�����и��Ӷ���
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
	// ������˾Ͳ��ټ�����
	if(mUseGrids.size() > 0)
		return mUseGrids;
	
	// ������x��z
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
		// ����ʹ�õ�������
		if(!aface[n])
			continue;

		// ֻ�к��ڷ���ͬʱ��Ч���ż���սǸ�
		cw = aface[ (n+1)%4 ] == 1;
		acw = aface[ (n-1+4)%4 ] == 1;

		// ����ʹ�ø���
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
					// ��x��ֻȡxmax�ľ���
					if(x != xmax) continue;
					// x+1��ض���Ч
					grid = flr->getGrid(x+1, z);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmin)��Ҫ��ȡ(xmax+1,zmin-1)�Ǹ�
					grid = flr->getGrid(x+1, z-1);
					if(cw && z == zmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmax)��Ҫ��ȡ(xmax+1,zmax+1)�Ǹ�
					grid = flr->getGrid(x+1, z+1);
					if(acw && z == zmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			case 1:
				{
					// ��-z��ֻȡzmin�ľ���
					if(z != zmin) continue;
					// z-1��ض���Ч
					grid = flr->getGrid(x, z-1);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmin)��Ҫ��ȡ(xmin-1,zmin-1)�Ǹ�
					grid = flr->getGrid(x-1, z-1);
					if(cw && x == xmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmin)��Ҫ��ȡ(xmax+1,zmin-1)�Ǹ�
					grid = flr->getGrid(x+1, z-1);
					if(acw && x == xmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			case 2:
				{
					// ��-x��ֻȡxmin�ľ���
					if(x != xmin) continue;
					// x-1��ض���Ч
					grid = flr->getGrid(x-1, z);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmax)��Ҫ��ȡ(xmin-1,zmax+1)�Ǹ�
					grid = flr->getGrid(x-1, z+1);
					if(cw && z == zmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmin)��Ҫ��ȡ(xmin-1,zmin-1)�Ǹ�
					grid = flr->getGrid(x-1, z-1);
					if(acw && z == zmin && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
				}
				break;
			case 3:
				{
					// ��z��ֻȡzmax�ľ���
					if(z != zmax) continue;
					// z+1��ض���Ч
					grid = flr->getGrid(x, z+1);
					if(grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmax,zmax)��Ҫ��ȡ(xmax+1,zmax+1)�Ǹ�
					grid = flr->getGrid(x+1, z+1);
					if(cw && z == zmax && grid && grid->getRoomID() == mRoom) mUseGrids.insert(grid);
					// (xmin,zmax)��Ҫ��ȡ(xin-1,zmax+1)�Ǹ�
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
	// ��Ϊ��Ϸ�߼�
	// ��ʱ�������жϿ��滻���

	return pThis->getHouseCompPropRow()->CanRep 
		&& (pThis->getHouseCompPropRow()->PropID == comp->getHouseCompPropRow()->PropID)
		//&& (pThis->getHouseCompRow()->ID != comp->getHouseCompRow()->ID)
		;	
}
//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
/** ��һ��grid�ϣ�Ѱ����Ʒ�ڷ�λ��
	@ ����ֵ -- ��һ�����������ڷ����ϵ����
*/
//-----------------------------------------------------------------------
std::pair<bool, const HouseSceneComponent *> HSCLayLogic::getFirstCanLayComponent( const HouseSceneComponent * pThis, 
																						  const HouseSceneGrid * grid ) 
{
	assert( grid );

	typedef std::pair<bool, const HouseSceneComponent *> ResultType;

	// �жϸ������Ƿ�ɷ���this��(��Щ���ֻ�ܷ������ڸ��ӣ���Щ���ֻ�ܷ����������)
	if ( !BitContains( grid->getHouseGridTypeRow()->AcceptLay, pThis->getHouseCompPropRow()->LayType ) )
	{
		return ResultType( false, NULL );
	}

	// ����߶�
	const int selfHeight = pThis->getHouseCompPropRow()->SelfHeight;

	const HouseSceneComponent * compDown = NULL;
	// �ڷŵײ�ֵ
	int hBottom = 0;

	const HouseSceneComponent * compUp = NULL;
	// �ڷŶ���ֵ
	int hTop = 0;
	
	HouseSceneGrid::ComponentList::const_iterator it = grid->getHouseSceneComponentList().begin();
	for ( ;	it != grid->getHouseSceneComponentList().end(); ++it )
	{
		//����ռ�ݲ�ͬ���ӿռ�����
		int layType = pThis->getHouseCompPropRow()->HoldType;
		int iteType = (*it)->getHouseCompPropRow()->HoldType;
		if(layType!=2 && iteType!=2 && layType!=iteType)
			continue;

		compUp = *it;
		hTop = compUp->getBottom();
		
		// ��������������
		if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
		{
			// ���������һ�����������ֹͣ����
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

	// ��������һ�����򽫶�����Ϊ���
	if ( it == grid->getHouseSceneComponentList().end() )
	{
		hTop = grid->getCreator()->getHeight();
		if ( hTop <= 0 )
			hTop = HOUSE_DEF_FLOOR_HEGHT;
	}
	
	// ��������������
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
/** �Զ���ǽ
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

	// �ڷŸ߶�Ĭ��Ϊƫ�Ƹ߶�
	height = pThis->getHouseCompPropRow()->OffsetHeight;

	// ȡ�õ�ǰthis->HoldGrid, ת��������GridsArray
	HouseCompPropRow::GridPosArray holdGrid = pThis->getHouseCompPropRow()->HoldGrid;
	HouseSceneComponent::convertGrids( holdGrid, face, grid->getGridX(), grid->getGridZ() );

	// ����this->SelfHeight + offsetHeight, �����Grid�����ʺϷ��õ�comp
	const int selfHeight = pThis->getHouseCompPropRow()->SelfHeight;
	std::pair<bool, const HouseSceneComponent *> ret = getFirstCanLayComponent( pThis, grid );
	if ( !ret.first )
	{
		return HRV_NO_LAY_SPACE;
	}

	const HouseSceneComponent * comp = ret.second;

	if ( comp )
	{
		// ��������������
		if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
		{
			// �Ƿ�ɷ���
			if ( !BitContains( comp->getHouseCompPropRow()->AcceptLay, pThis->getHouseCompPropRow()->LayType ) )
			{
				return HRV_NO_MATCH_LAYTYPE;
			}

			// ȡ��comp->LayGrid, ת��������GridsArray;
			HouseCompPropRow::GridPosArray layGrid = comp->getHouseCompPropRow()->LayGrid;

			const HouseSceneGrid * grid0 = comp->getAttachGrid();

			int x, z;
			grid0->getGridNo( x, z );
			HouseSceneComponent::convertGrids( layGrid, comp->getFace(), x, z );

			// ��֤LayGrid ����HoldGrid
			for ( HouseCompPropRow::GridPosArray::const_iterator it = holdGrid.begin();
				it != holdGrid.end();
				++it )
			{
				if ( std::find( layGrid.begin(), layGrid.end(), *it ) == layGrid.end() )
					return HRV_NO_GRID_LAY;
			}		
		}
	}

	// ȡ�����и��Ӷ���
	std::vector<const HouseSceneGrid *> gs = pThis->_calcHoldGrids( grid, face );
	
	// ���holdGrid ��ÿ�����ӣ���comp�߶��϶��пռ��������this
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

		// ��������������
		if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
		{
			// ֻ�ܷ���ͬһ���������
			if ( r.second != comp )
				return HRV_NO_LAY_SPACE;
		}
	}	

	//��������û��ǽ���赲(��ǽ����������ڲ���ǽ��)
	for ( int x = pThis->mHoldBox.mXmin; x <= pThis->mHoldBox.mXmax; ++x )
	{
		for ( int z = pThis->mHoldBox.mYmin; z <= pThis->mHoldBox.mYmax; ++z )
		{
			const HouseSceneGrid * p = gs[pThis->mHoldBox.linearIndex(x,z)];
			if ( x != pThis->mHoldBox.mXmax )
			{
				// ����ǿ�ǽ�������Ϊ��ǽ���ӣ����ü�����ǽ
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

	// ����ǿ�ǽ���
	if ( pThis->isLayWallObject() )
	{
		// ȡ��ǽ����
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

		// �ж��Ƿ�ǽ
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

	// ��������������
	if ( pThis->getHouseCompPropRow()->OffsetHeight == 0 ) 
	{
		// �ڷŸ߶�
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

	// ������������ֱ�ӷ���ok
	if ( pThis->getHouseCompPropRow()->OffsetHeight != 0 )
	{
		return HRV_OK;
	}

	// ��������ж���,����ʰȡ
	const HouseSceneGrid * gridAttach = pThis->getAttachGrid();
	// �ҳ�����LayGrid, ��ÿ�����Ӽ��������û����Ʒ�ڷ�
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





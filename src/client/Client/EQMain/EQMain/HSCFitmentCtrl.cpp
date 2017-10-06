#include "EQMainPCH.h"

#include "HSCFitmentCtrl.h"
#include "SystemGcs.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "Commfunc.h"

//-----------------------------------------------------------------------
HSCFitmentCtrl::HSCFitmentCtrl() 
	: mComp(NULL)
	, mRoting(false)
	, mFace(0)
	, mLastOperateGrid(NULL)
	, mListener(NULL)
	, mPutMode( pmEditor )
{
	mSceneNode = SystemGcs::getSingleton().getSceneManager()->createSceneNode();
}
//-----------------------------------------------------------------------
HSCFitmentCtrl::~HSCFitmentCtrl()
{
	assert( !mComp );

	//mSceneNode->removeAndDestroyAllChildren();
	mSceneNode->getCreator()->destroySceneNode(mSceneNode);
}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::attach(HouseSceneComponent* hsc)
{
	assert( !mComp );

	mRoting = false;
	mFace = 0;
	mSceneNode->resetOrientation();
	mLastOperateGrid = NULL;

	//
	mComp = hsc;
	//enterScene();
	mComp->setParentNode(mSceneNode);
	mCompGridGraphics.attach(mSceneNode, mComp->getHouseCompPropRow());
	mFace = mComp->getFace();
}
//-----------------------------------------------------------------------
HouseSceneComponent* HSCFitmentCtrl::detach()
{
	HouseSceneComponent* ret = mComp;
	if ( !mComp )
		return NULL;

	mComp->setParentNode(NULL);
	leaveScene();
	mComp = NULL;

	//
	mRoting = false;
	mFace = 0;
	mSceneNode->resetOrientation();
	mLastOperateGrid = NULL;

	return ret;
}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::exec(const Ogre::Ray& ray)
{
	if ( !mComp )
		return;

	HouseSceneGrid* grid;
	Ogre::Vector3 cp;

	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	sce->rayGrid(ray, grid, cp);

	if ( !mRoting )
	{
		if ( grid )
		{
			enterScene();

			if ( calcLay(grid) )
			{
				mSceneNode->setPosition(grid->getSceneNode()->_getDerivedPosition());
			}
			else
				mSceneNode->setPosition(cp);

			mLastOperateGrid = grid;
		}
	}
	else // ������ת
	{
		if ( mLastOperateGrid )
		{
			enterScene();

			if ( grid != mLastOperateGrid )	//����ͬһ������( �˴���grid �п���ΪNULL )
			{
				Ogre::Vector3 dir = cp - mLastOperateGrid->getSceneNode()->_getDerivedPosition();
				Ogre::Quaternion q(Ogre::Radian(Ogre::Degree(45)), Ogre::Vector3::UNIT_Y);
				dir = q * dir;
				int oldFace = mFace;
				if ( dir.x > 0 && dir.z < 0 )
					mFace = 0;
				else if ( dir.x < 0 && dir.z < 0 )
					mFace = 1;
				else if ( dir.x < 0 && dir.z > 0 )
					mFace = 2;
				else if ( dir.x > 0 && dir.z > 0 )
					mFace = 3;

				mSceneNode->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90*mFace)), Ogre::Vector3::UNIT_Y));

				if ( oldFace != mFace && mListener )
					mListener->notifyFaceChange(mFace);
			}

			calcLay(mLastOperateGrid);
		}
	}

}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::rotStart()
{
	if ( !mComp )
		return;

	mRoting = true;
}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::rotEnd()
{
	if ( !mComp )
		return;

	mRoting = false;
}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::rotate(float degree)
{
// 	Ogre::Quaternion q(Ogre::Radian(Ogre::Degree(degree)), Ogre::Vector3::UNIT_Y);
// 	mSceneNode->rotate(q);

	if(degree < 0)
		mFace--;
	else if(degree > 0)
		mFace++;

	if(mFace < 0)
		mFace +=4;
	else if(mFace >=4)
		mFace -=4;

	mSceneNode->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90*mFace)), Ogre::Vector3::UNIT_Y));
}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::enterScene()
{
	if ( !mSceneNode->getParentSceneNode() )
		SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->addChild(mSceneNode);
}
//-----------------------------------------------------------------------
void HSCFitmentCtrl::leaveScene()
{
	if ( mSceneNode->getParentSceneNode() )
		mSceneNode->getParentSceneNode()->removeChild(mSceneNode);
}
//-----------------------------------------------------------------------
bool HSCFitmentCtrl::calcLay( HouseSceneGrid * grid )
{
	bool ret;

	mSceneNode->setOrientation( Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90*mFace) ), Ogre::Vector3::UNIT_Y));

	int flrNum, x, z;
	flrNum = grid->getCreator()->getFloorNO();
	grid->getGridNo(x,z);

	int height = 0;
	std::vector<HouseSceneGrid *> gs;
	int face = mFace;
	HouseReturnValue hrv = HRV_FAIL;
	HouseSceneComponent * compRep = NULL;

	bool beProcessed = false; // ��������
	if ( mPutMode == pmPlayer ) // ���ģʽ��
	{
		// ���滻��Ʒֻ�����滻��������ڷ�
		if ( mComp->getHouseCompPropRow()->CanRep ) // ������滻�������
		{
			// �ҿɱ��滻�����
			compRep = HSCLayLogic::findCanReplaceComponent( mComp, grid );
			if ( compRep ) // �ҵ�
			{
				face = compRep->getFace();
				height = mComp->getBottom();
				hrv = HRV_OK;
			}
			else // û�ҵ�
				hrv = HRV_FAIL; 
			beProcessed = true;
		}
	}

	if ( !beProcessed ) // ���û�������
	{
		if ( mComp->isLayWallObject() ) // ���ж��Ƿ�ǽ����
		{
			// �Զ���ǽ
			hrv = HSCLayLogic::canAutoLayWall( mComp, grid, face, height );
		}
		else 
			hrv = HSCLayLogic::canLay( mComp, grid, face, height );
	}

	if ( HRV_OK == hrv )
	{
		mFace = face;
		if ( compRep )
			mCompGridGraphics.showYellowHoldGrid();
		else
			mCompGridGraphics.showGreenHoldGrid();

		//
		if ( mListener )
			mListener->notifyCanLay( grid, mFace, height, compRep );

		ret = true;
	}
	else
	{
		mCompGridGraphics.showRedHoldGrid();

		//
		if ( mListener )
			mListener->notifyForbidLay(hrv);

		ret = false;
	}

	if ( mComp->getHouseCompPropRow()->OffsetHeight == 0)
		mComp->setPosition( Ogre::Vector3( 0, height, 0 ) );
	else
		mComp->setPosition( Ogre::Vector3::ZERO );

	return ret;
}






//-----------------------------------------------------------------------
std::string HSCFitmentHelper::fetchResName()
{
	//��ʱ��������	
	static int num = 1;
	++num;
	return "FitmentAuto" + GetCurDateTimeStr() + Ogre::StringConverter::toString( num );
}
//-----------------------------------------------------------------------
std::string HSCFitmentHelper::getFitmentErrorStr( int err )
{
	std::string strErr;
	switch(err)
	{
	case HRV_OK:
		strErr = "�ɹ�";
		break;

	case HRV_LAYED:
		strErr = "������Ѿ��ڷ�";
		break;

	case HRV_NO_GRID_LAY:
		strErr = "û���㹻��������Է���";
		break;

	case HRV_WALL_STOP:
		strErr = "��ǽ��ס��";
		break;

	case HRV_NEAR_WALL:
		strErr = "�������Ҫ��ǽ�ڷ�";
		break;

	case HRV_NO_LAY_SPACE:
		strErr = "û�аڷſռ�";
		break;

	case HRV_NO_LAYED:
		strErr = "�������δ�ڷ�";
		break;

	case HRV_NOTAKE_FRAME:
		strErr = "��ܲ���������ȡ";
		break;

	case HRV_HAS_COMP_LAY:
		strErr = "������ڷ�������";
		break;

	case HRV_NO_AUTO_LAYWALL:
		strErr = "�����Զ���ǽ";
		break;

	case HRV_NO_MATCH_LAYTYPE:
		strErr = "�������Բ���";
		break;

	default:
		strErr = "�������" + Ogre::StringConverter::toString((int)err);
		break;
	}

	return strErr;
}

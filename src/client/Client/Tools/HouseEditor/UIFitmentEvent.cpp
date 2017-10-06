#include "HouseEditorPCH.h"

#include "UIFitmentEvent.h"
#include "SceneManager.h"
#include "MfcInputEventMgr.h"
#include "HSCManager.h"
#include "SystemGcs.h"
#include "MainFrm.h"
#include "CommFunc.h"
#include "CollisionTools.h"
#include "HouseScene.h"
#include "SceneEntityObject.h"

//-----------------------------------------------------------------------
static Ogre::Ray getRay( CPoint point )
{
	Ogre::Camera * cam = GraphicsSystem::getSingleton().getCamera();

	int w = cam->getViewport()->getActualWidth();
	int h = cam->getViewport()->getActualHeight();

	Ogre::Ray ray = cam->getCameraToViewportRay((float)point.x/w, (float)point.y/h);

	return ray;
}
//-----------------------------------------------------------------------
static PickObject* getPickObjectByPoint( CPoint point )
{
	const Ogre::Ray kRay = getRay(point);

	Ogre::Vector3 result;
	Ogre::MovableObject* target;
	float closest_distance;
	MOC::CollisionTools::getSingleton().raycast(kRay, result, target, closest_distance);

	if ( target )
	{
		PickObject* pick = PickObjectProxy::pickUpObject(target);
		return pick;
	}

	return NULL;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
EditFitmentEvent * EditFitmentEvent::create()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert(sce);
	return new EditFitmentEvent( &sce->getFitmentCtrl() );
}
//-----------------------------------------------------------------------
EditFitmentEvent::EditFitmentEvent( HSCFitmentCtrl * c ) 
	: MfcInputEvent( epEditFitment )
	, mFitment(NULL)
	, mFitmentCtrl( c )
	, mCompRep(NULL)
	, mCanLay(false)
	, mErr(HRV_FAIL)
	, mFace(0)
	, mGrid(NULL)
	, mOldGrid(NULL)
	, mMode( OpEditFitment )
	, mPickingFitment(false)
	, mLastCompID(0)
	, mBottom(0)
	, mOldBottom(0)
{	
	assert( mFitmentCtrl );
}
//-----------------------------------------------------------------------
EditFitmentEvent::~EditFitmentEvent()
{
	undo();
}
//-----------------------------------------------------------------------
void EditFitmentEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	if ( mFitment )
	{
		mFitmentCtrl->rotStart();
	}
	else
	{
		HouseSceneComponent* comp=NULL;
		PickObject* pick = getPickObjectByPoint(point);
		if ( pick->getClassName() == "HouseSceneComponent" )
			comp = dynamic_cast<HouseSceneComponent*>(pick);

		if ( comp )
		{
			HouseReturnValue hrv = HSCLayLogic::canTake(comp);

			// 如果是可替换物件，只有在编辑器模式下才可拾取
			if ( comp->getHouseCompPropRow()->CanRep )
			{
				if ( this->mFitmentCtrl->getPutMode() != HSCFitmentCtrl::pmEditor )
					hrv = HRV_NOTAKE;
			}

			if ( hrv == HRV_OK )
			{
				// 拾取物件
				pickupFitment(comp, point);	//添加事件了

				mPickingFitment = true;
			}
			else
			{
				//提示
				g_MainFrame->showError( HSCFitmentHelper::getFitmentErrorStr(hrv) ); 
			}
		}
	}
	processed = true;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::LButtonUp( UINT nFlags, CPoint point, bool & processed )
{
	if ( !mFitment )
		return;

	if ( mPickingFitment )
	{
		mPickingFitment = false;
		return;
	}

	Ogre::Ray ray = getRay(point);

	mFitmentCtrl->exec(ray);
	mFitmentCtrl->rotEnd();

	//
	if ( mCanLay )
	{
		// 处理替换操作
		if ( mCompRep )
		{
			mFace = mCompRep->getFace();
			mGrid = mCompRep->getAttachGrid();

			// 删除该位置上的旧物体
			g_ResourceDlg->OnRemoveComponent( mCompRep );
			HSCManager::getSingleton().destroyHouseSceneComponent( mCompRep );
			mCompRep = NULL;
		}

		mFitmentCtrl->detach();

		{
			mFitment->lay( mGrid, mFace, mBottom );
			g_ResourceDlg->OnAddComponent( mFitment );
			//g_ResourceDlg->selectComponent( mFitment );
			mFitment->turnOnLight();
		}

		mFitment = NULL;

		mCanLay = false;
		mErr = HRV_FAIL;
		if ( OpNewFitment == mMode )
		{
			//创建新的
			createNewFitment( mLastCompID );
		}
	}
	else
	{
		g_MainFrame->showError( HSCFitmentHelper::getFitmentErrorStr(mErr) ); 
	}
	processed = true;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	if ( !mFitment )
		return;

	Ogre::Ray ray = getRay(point);

	if ( GetAsyncKeyState(VK_LBUTTON) & 0x8000 )
	{		
		if ( delta.x * delta.x + delta.y * delta.y > 5 )
		{
			// 此二行代码作用：物件拾取之后，不需要再mouse down，就可以旋转了
			mFitmentCtrl->rotStart();
			this->mPickingFitment = false;
		}
	}

	mFitmentCtrl->exec(ray);
	processed = true;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		undo();
		processed = true;
	}
	else if ( VK_DELETE == nChar )
	{
		delFitment();
		mMode = OpEditFitment;
		processed = true;
	}
}
//-----------------------------------------------------------------------
void EditFitmentEvent::notifyCanLay( HouseSceneGrid * grid, int face, int height, HouseSceneComponent * compRep )
{
	mCanLay = true;
	mGrid = grid;
	mFace = face;
	mBottom = height;
	mCompRep = compRep;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::notifyForbidLay(HouseReturnValue hrv)
{
	mCanLay = false;
	mErr = hrv;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::undo()
{
	mCanLay = false;
	mErr = HRV_FAIL;

	if ( mFitmentCtrl )
	{
		mFitmentCtrl->detach();
		mFitmentCtrl->setListener(NULL);
	}

	if ( mFitment )
	{
		// 如果是新增物件，则删除物件; 如果是移动物件，则将物件放回原处		
		if ( OpNewFitment == mMode )
			HSCManager::getSingleton().destroyHouseSceneComponent(mFitment);
		else
		{
			mFitment->lay( mOldGrid, mOldFace, mOldBottom );
			g_ResourceDlg->OnAddComponent( mFitment );
		}

		mFitment = NULL;
	}

	mMode = OpEditFitment;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::delFitment()
{
	mCanLay = false;
	mErr = HRV_FAIL;

	if ( mFitmentCtrl )
	{
		mFitmentCtrl->detach();
		mFitmentCtrl->setListener(NULL);
	}

	if ( mFitment )
	{
		HSCManager::getSingleton().destroyHouseSceneComponent(mFitment);
		mFitment = NULL;
	}
}
//-----------------------------------------------------------------------
void EditFitmentEvent::pickupFitment(HouseSceneComponent* comp, CPoint point)
{
	assert(!mFitment);
	mFitment = comp;
	mOldGrid = comp->getAttachGrid();
	mOldFace = comp->getFace();
	mOldBottom = comp->getBottom();

	{
		g_ResourceDlg->OnRemoveComponent( mFitment );
		mFitment->take();
	}

	mFitmentCtrl->attach(mFitment);
	mFitmentCtrl->setListener(this);
	mFitmentCtrl->exec( getRay(point) );
	mMode = OpEditFitment;
}
//-----------------------------------------------------------------------
void EditFitmentEvent::createNewFitment( uint compID )
{
	if ( 0 == compID )
		return;

	undo();

	mFitment = HSCManager::getSingleton().createHouseSceneComponent();
	mFitment->parseRes(compID);
	mFitment->load();

	
	mFitment->setResName(HSCFitmentHelper::fetchResName());

	mFitmentCtrl->attach(mFitment);
	mFitmentCtrl->setListener(this);

	// 记录最后的compID
	mLastCompID = compID;

	// 设为新增模式
	mMode = OpNewFitment;
}
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
BrowseComponentEvent::BrowseComponentEvent() 
	: MfcInputEvent(epBrowseComponent)
	, mLastComp(NULL)
{
	g_ResourceDlg->clearProperty();
}
//-----------------------------------------------------------------------
BrowseComponentEvent::~BrowseComponentEvent()
{
	if ( mLastComp )
		mLastComp->getModel()->getMainNode()->showBoundingBox( false );
	g_MainFrame->UnCheckedToolBarItem( ID_BUTTON_COMPONENT );
	g_ResourceDlg->clearProperty();
}
//-----------------------------------------------------------------------
BrowseComponentEvent * BrowseComponentEvent::create()
{
	return new BrowseComponentEvent();
}
//-----------------------------------------------------------------------
void BrowseComponentEvent::setBrowseComponent( HouseSceneComponent * comp )
{
	if ( mLastComp )
		mLastComp->getModel()->getMainNode()->showBoundingBox( false );

	comp->getModel()->getMainNode()->showBoundingBox( true );
	mLastComp = comp;
}
//-----------------------------------------------------------------------
void BrowseComponentEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec( point );
	processed = true;
}
//-----------------------------------------------------------------------
bool BrowseComponentEvent::exec(CPoint point)
{
	HouseSceneComponent* comp=NULL;
	PickObject* pick = getPickObjectByPoint(point);
	if ( pick->getClassName() == "HouseSceneComponent" )
		comp = dynamic_cast<HouseSceneComponent*>(pick);

	if ( comp )
		g_ResourceDlg->selectComponent( comp );

	return false;
}
//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
SetLightEvent::SetLightEvent() 
: MfcInputEvent(epBrowseComponent)
, mLastComp(NULL)
, mLastObj(NULL)
{

}
//-----------------------------------------------------------------------
SetLightEvent::~SetLightEvent()
{
	if (mLastComp)
		mLastComp->getModel()->getMainNode()->showBoundingBox(false);

	if(mLastObj)
		mLastObj->showBoundingBox(false);

	g_MainFrame->UnCheckedToolBarItem( ID_BUTTON_LIGHT );
}
//-----------------------------------------------------------------------
SetLightEvent * SetLightEvent::create()
{
	return new SetLightEvent();
}
//-----------------------------------------------------------------------
void SetLightEvent::setLightComponent(HouseSceneComponent* comp)
{
	if(!comp->getHouseCompPropRow()->LightID)
		return;

	if (mLastComp)
		mLastComp->getModel()->getMainNode()->showBoundingBox(false);

	comp->getModel()->getMainNode()->showBoundingBox(true);

	mLastComp = comp;
}
//-----------------------------------------------------------------------
void SetLightEvent::setLightSceneEntityObject(SceneEntityObject* obj)
{
	if(!obj->getLightId())
		return;

	if (mLastObj)
		mLastObj->showBoundingBox(false);

	obj->showBoundingBox(true);

	mLastObj = obj;
}
//-----------------------------------------------------------------------
void SetLightEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	exec( point );
	processed = true;
}
//-----------------------------------------------------------------------
bool SetLightEvent::exec(CPoint point)
{
	PickObject* pick = getPickObjectByPoint(point);
	if (pick->getClassName() == "HouseSceneComponent")
	{
		HouseSceneComponent* comp = dynamic_cast<HouseSceneComponent*>(pick);
		if (comp)
		{
			setLightComponent(comp);
			g_ResourceDlg->showLightProperty(comp);
		}
	}
	else if(pick->getClassName() == "SceneEntityObject")
	{
		SceneEntityObject* obj = dynamic_cast<SceneEntityObject*>(pick);
		if(obj)
		{
			setLightSceneEntityObject(obj);
			g_ResourceDlg->showLightProperty(obj);
		}
	}

	return false;
}
//-----------------------------------------------------------------------

#include "HouseEditorPCH.h"

#include "UISceneObjectEvent.h"
#include "MfcInputEventMgr.h"
#include "HouseEditor.h"
#include "SceneObjectDlg.h"
#include "RootManager.h"
#include "TerrainData.h"
#include "SceneEntityObject.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "GraphicsSystem.h"
#include "PickObjectProxy.h"
#include "CollisionTools.h"
#include "SceneInfo.h"
#include "SceneObjectOperator.h"
#include "OperatorManager.h"

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
static PickObject* getPickObject(CPoint point, Ogre::Vector3& result)
{
	PickObject* pick = NULL;

	const Ogre::Ray kRay = getRay(point);

	//Ogre::Vector3 result;
	Ogre::MovableObject* target;
	float closest_distance;
	if ( !MOC::CollisionTools::getSingleton().raycast(kRay, result, target, closest_distance) )
		return NULL;

	if ( target )
	{
		pick = PickObjectProxy::pickUpObject(target);
	}

	return pick;
}


//-----------------------------------------------------------------------
CreateSceneEntityObjectEvent::CreateSceneEntityObjectEvent() 
: MfcInputEvent( epSceneObject ),
mEntityObjId(0),
mSceneEntityObject(NULL),
mLight(0),
mParticle(0)
{
}
//-----------------------------------------------------------------------
CreateSceneEntityObjectEvent::~CreateSceneEntityObjectEvent()
{
	if ( mSceneEntityObject )
	{
		delete mSceneEntityObject;
		mSceneEntityObject = NULL;
	}

	g_CSceneObjectDlg->unselectTree();
}
//-----------------------------------------------------------------------
CreateSceneEntityObjectEvent * CreateSceneEntityObjectEvent::create()
{
	return new CreateSceneEntityObjectEvent();
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::setResource(const std::string& mesh, const std::string& mat, uint light, uint particle, int cut)
{
	mMeshName = mesh;
	mMatName = mat;
	mLight = light;
	mParticle = particle;
	mCutDistanceLevel = cut;

	assert(mSceneEntityObject==NULL);
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::setResource(uint id)
{
	mEntityObjId = id;

	assert(mSceneEntityObject==NULL);
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	processed = true;

	if ( !mSceneEntityObject )
		return;

	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();

	scene->addSceneObject(mSceneEntityObject);

	doFinish();

	mSceneEntityObject = NULL;
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::doFinish()
{
	CreateSceneEntityObjectOperator* op = new CreateSceneEntityObjectOperator;
	op->add(mSceneEntityObject, true);
	RootManager::getSingleton().getOperatorManager()->addOperator(op);
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::RButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	processed = true;

	MfcInputEventMgr::getSingleton().delEvent<CreateSceneEntityObjectEvent>();
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	Ogre::Vector3 position;
	Ogre::Vector2 pt(point.x, point.y);
	bool intersected = RootManager::getSingleton().getTerrainIntersects(pt, position);
	if (!intersected)
	{
		return;
	}

	if ( !mSceneEntityObject )
	{
		mSceneEntityObject = new SceneEntityObject();

		if (mEntityObjId == 0)
		{
			mSceneEntityObject->setMesh(mMeshName);
			mSceneEntityObject->setMaterial(mMatName);
			mSceneEntityObject->setLightId(mLight);
			mSceneEntityObject->setCutDistanceLevel(mCutDistanceLevel);
			mSceneEntityObject->setParticleId(mParticle);
		}
		else
		{
			mSceneEntityObject->setEntityObjId(mEntityObjId);
		}

		mSceneEntityObject->createInst();
	}

	mSceneEntityObject->setPosition(position);
}
//-----------------------------------------------------------------------
void CreateSceneEntityObjectEvent::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateSceneEntityObjectEvent>();
		processed = true;
	}
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
SceneObjectTransform::SceneObjectTransform() 
: MfcInputEvent( epSceneObject )
, mCurrTransMode(SELECT)
, mCurrSceneObject(NULL)
, mHitObject(false)
, mOffsetObject(Ogre::Vector3::ZERO)
, mOrgId(0)
{
	EQSceneManager::getSingleton().getSceneInfo()->addListener(this);
}
//-----------------------------------------------------------------------
SceneObjectTransform::~SceneObjectTransform()
{
	changeScenObject(NULL);

	EQSceneManager::getSingleton().getSceneInfo()->removeListener(this);

	g_CSceneObjectDlg->changeTransformButton("");
}
//-----------------------------------------------------------------------
SceneObjectTransform * SceneObjectTransform::create()
{
	return new SceneObjectTransform();
}
//-----------------------------------------------------------------------
void SceneObjectTransform::initTransMode(TransMode mode)
{
	mCurrTransMode = mode;
}
//-----------------------------------------------------------------------
void SceneObjectTransform::changeTransMode(TransMode mode)
{
	mCurrTransMode = mode;

	switch(mCurrTransMode)
	{
	case SELECT:
		g_CSceneObjectDlg->changeTransformButton("select");
		break;
	case MOVE:
		g_CSceneObjectDlg->changeTransformButton("move");
		break;
	case ROTATION:
		g_CSceneObjectDlg->changeTransformButton("rotation");
		break;
	case SCALE:
		g_CSceneObjectDlg->changeTransformButton("scale");
		break;
	}
}
//-----------------------------------------------------------------------
void SceneObjectTransform::changeScenObject(SceneObject* newObj)
{
	if ( mCurrSceneObject == newObj )
		return;

	if ( mCurrSceneObject )
		mCurrSceneObject->showBoundingBox(false);

	mCurrSceneObject = newObj;

	if ( mCurrSceneObject )
		mCurrSceneObject->showBoundingBox(true);
}
//-----------------------------------------------------------------------
void SceneObjectTransform::LButtonDown( UINT nFlags, CPoint point, bool & processed )
{
	processed = true;

	Ogre::Vector3 offset;
	PickObject* pickObj = getPickObject(point, offset);
	SceneObject* obj = dynamic_cast<SceneObject*>(pickObj);


	if ( obj )
	{
		offset = offset - obj->_getDerivedPosition();

		changeScenObject(obj);
		mHitObject = true;
		mOffsetObject = offset;

		mOrgId = obj->getId();
		mOrgPos = obj->getPosition();
		mOrgRot = obj->getOrientation();
		mOrgScale = obj->getScale();
	}
	else
	{
		if ( mCurrTransMode == SELECT )
			changeScenObject(NULL);

		mHitObject = false;
		mOffsetObject = Ogre::Vector3::ZERO;
	}

	//复位数据
	mRotReferValue = 0;
}
//-----------------------------------------------------------------------
void SceneObjectTransform::LButtonUp( UINT nFlags, CPoint point, bool & processed )
{
	processed = true;

	mHitObject = false;
	mOffsetObject = Ogre::Vector3::ZERO;

	if ( mCurrSceneObject && mCurrSceneObject->getId() == mOrgId )
	{
		if ( mCurrSceneObject->getPosition() != mOrgPos || mCurrSceneObject->getOrientation() != mOrgRot || mCurrSceneObject->getScale() != mOrgScale )
		{
			SceneObjectTransformOperator* op = new SceneObjectTransformOperator;
			op->add(mCurrSceneObject, mOrgPos, mOrgRot, mOrgScale);
			RootManager::getSingleton().getOperatorManager()->addOperator(op);
		}
	}

	mOrgId = 0;
}
//-----------------------------------------------------------------------
void SceneObjectTransform::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	if ( !(GetAsyncKeyState(VK_LBUTTON) & 0x8000) )
	{
		mHitObject = false;
		mOffsetObject = Ogre::Vector3::ZERO;
	}

	if ( !mCurrSceneObject )
		return;


	switch(mCurrTransMode)
	{
	case SELECT:	//目前没作用, 以后或许会有多选的效果
		break;
	case MOVE:
		moveSecneObject(delta, point);
		break;
	case ROTATION:
		rotationSceneObject(delta, point);
		break;
	case SCALE:
		scaleSceneObject(delta, point);
		break;
	}

}
//-----------------------------------------------------------------------
static void fixFloat(float& f)
{
	int a = (int)Ogre::Math::Floor(f/20.0f);
	if ( a % 2 == 0 )	//偶数
		f = a*20.0f;
	else
		f = (a+1)*20.0f;
}
static void fixGrid(Ogre::Vector3& position)
{
	fixFloat(position.x);
	fixFloat(position.z);
}
//-----------------------------------------------------------------------
void SceneObjectTransform::moveSecneObject(CPoint delta, CPoint point)
{
	assert(mCurrSceneObject);
	if ( !mHitObject )	// 必然在鼠标点击状态
		return;

	if ( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	//贴地移动
	{
		Ogre::Vector3 position;
		Ogre::Ray ray = getRay( point );
		ray.setOrigin(ray.getOrigin()-mOffsetObject);
		bool intersected = RootManager::getSingleton().getTerrainIntersects(ray, position);
		if (intersected)
		{
			if ( GetAsyncKeyState(VK_MENU) & 0x8000) 
				fixGrid(position);
			mCurrSceneObject->setPosition(position);
		}
	}
	else if ( GetAsyncKeyState(VK_SHIFT) & 0x8000 )	//垂直移动
	{
		Ogre::Ray ray = getRay( point );
		ray.setOrigin(ray.getOrigin()-mOffsetObject);
		Ogre::Vector3 v = mCurrSceneObject->_getDerivedPosition();

		Ogre::Vector3 dir = ray.getDirection();
		dir = -dir;
		dir.y = 0;
		if ( dir != Ogre::Vector3::ZERO ) //不是垂直xz平面
		{
			Ogre::Plane pla(dir, v);
			std::pair<bool, float> rt = Ogre::Math::intersects(ray, pla);
			if ( rt.first )	// 如果有相交
			{
				Ogre::Vector3 worldCrossingPoint = ray.getPoint( rt.second );
				worldCrossingPoint.x = v.x;
				worldCrossingPoint.z = v.z;

				mCurrSceneObject->setPosition(worldCrossingPoint);
			}
		}
	}
	else											//水平移动
	{
		Ogre::Ray ray = getRay( point );
		ray.setOrigin(ray.getOrigin()-mOffsetObject);
		Ogre::Vector3 v = mCurrSceneObject->_getDerivedPosition();
		Ogre::Plane pla( Ogre::Vector3::UNIT_Y, v );
		std::pair<bool, float> rt = Ogre::Math::intersects(ray, pla);
		if ( rt.first )	// 如果有相交
		{
			Ogre::Vector3 worldCrossingPoint = ray.getPoint( rt.second );

			if ( GetAsyncKeyState(VK_MENU) & 0x8000) 
				fixGrid(worldCrossingPoint);
			mCurrSceneObject->setPosition(worldCrossingPoint);
		}
	}

}
//-----------------------------------------------------------------------
//四舍五入
float RoundEx(const float& dIn)
{
	if (dIn >= 0.0) 
	{
		return int(dIn + 0.5);
	}
	else
	{
		return int(dIn - 0.5);
	}
}
void SceneObjectTransform::rotationSceneObject(CPoint delta, CPoint point)
{
	assert(mCurrSceneObject);
	if ( !mHitObject )
		return;


	if ( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	//
	{
		const float factor = 0.02f*180.0f/3.14159f;
		mRotReferValue += factor*delta.x;

		float a = abs(mRotReferValue);
		int n = 0;
		while(a>=45.0)
		{
			a-=45.0;
			++n;
		}

		if (n!=0)
		{		
			if ( mRotReferValue >= 0 )
				mRotReferValue = a;
			else
			{
				mRotReferValue = -a;
				n = -n;
			}


			const Ogre::Quaternion& que = mCurrSceneObject->getOrientation();
			float deg = que.getYaw().valueDegrees();
			deg = RoundEx(deg);
			deg = 45*((int)(deg/45));
			deg += 45*n;

			Ogre::Quaternion q;
			q.FromAngleAxis(Ogre::Radian(Ogre::Degree(deg)),Ogre::Vector3::UNIT_Y);
			mCurrSceneObject->setOrientation(q);

		}
	}
	else											//z旋转
	{
		const float factor = 0.02f;
		float val = delta.x;
		val = factor*val;

		mCurrSceneObject->yaw(Ogre::Radian(val));
	}
}
//-----------------------------------------------------------------------
void SceneObjectTransform::scaleSceneObject(CPoint delta, CPoint point)
{
	assert(mCurrSceneObject);
	if ( !mHitObject )
		return;

	const float factor = 0.1f;
	float val = delta.x*factor;
	float v;
	if ( val > 0 )
		v = Ogre::Math::Pow(1/0.9f, val);
	else
		v = Ogre::Math::Pow(0.9f, -val);
	Ogre::Vector3 scale = mCurrSceneObject->getScale();
	scale.x *= v;
	if ( scale.x < 0.1f )
		scale.x = 0.1f;
	scale.y *= v;
	if ( scale.y < 0.1f )
		scale.y = 0.1f;
	scale.z *= v;
	if ( scale.z < 0.1f )
		scale.z = 0.1f;

	mCurrSceneObject->setScale(scale);
}
//-----------------------------------------------------------------------
void  SceneObjectTransform::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		MfcInputEventMgr::getSingleton().delEvent<SceneObjectTransform>();
		processed = true;
	}
	else if ( 'Q' == nChar )
	{
		changeTransMode(SELECT);
		processed = true;
	}
	else if ( 'W' == nChar )
	{
		changeTransMode(MOVE);
		processed = true;
	}
	else if ( 'E' == nChar )
	{
		changeTransMode(ROTATION);
		processed = true;
	}
	else if ( 'R' == nChar )
	{
		changeTransMode(SCALE);
		processed = true;
	}
	else if ( VK_DELETE == nChar )
	{
		delSelectObject();
		processed = true;
	}
	//else if ( 'D' == nChar )
	//{
	//	const Ogre::RenderTarget::FrameStats& stats = GraphicsSystem::getSingleton().getWindow()->getStatistics();
	//	processed = true;
	//}
	//else if ( 'F' == nChar )
	//{
	//	const Ogre::RenderTarget::FrameStats& stats = GraphicsSystem::getSingleton().getWindow()->getStatistics();
	//	EQSceneManager::getSingleton().getSceneInfo()->bakeStaticGeometries(80*32);
	//	processed = true;
	//}
}
//-----------------------------------------------------------------------
void SceneObjectTransform::onDestroySceneObject(uint id)
{
	if ( mCurrSceneObject && mCurrSceneObject->getId() == id )
	{
		changeScenObject(NULL);
		mHitObject = false;
		mOffsetObject = Ogre::Vector3::ZERO;
	}
}
//-----------------------------------------------------------------------
void SceneObjectTransform::delSelectObject()
{
	if ( !mCurrSceneObject )
		return;

	SceneObject* delObj = mCurrSceneObject;

	changeScenObject(NULL);
	mHitObject = false;
	mOffsetObject = Ogre::Vector3::ZERO;

	delOperate(delObj);

	//
	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();
	scene->destroySceneObject(delObj->getId());
	delObj = NULL;

}
//-----------------------------------------------------------------------
void SceneObjectTransform::delOperate(SceneObject* obj)
{
	if ( obj->getClassName() == "SceneEntityObject" )
	{
		SceneEntityObject* o = static_cast<SceneEntityObject*>(obj);
		CreateSceneEntityObjectOperator* op = new CreateSceneEntityObjectOperator;
		op->add(o, false);
		RootManager::getSingleton().getOperatorManager()->addOperator(op);

	}
	else
		assert(0);
}

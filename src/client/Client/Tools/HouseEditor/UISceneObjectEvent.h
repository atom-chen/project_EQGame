/******************************************************************************
* 作者: Y3
* 时间: 2009-07-12
* 描述: 场景物件窗口的对应事件
******************************************************************************/
#ifndef UISceneObjectEvent_H
#define UISceneObjectEvent_H

#include "MfcInputEvent.h"

#include "SceneInfo.h"

class SceneEntityObject;
class SceneObject;
class PickObject;

class CreateSceneEntityObjectEvent : public MfcInputEvent
{
public:
	explicit CreateSceneEntityObjectEvent();
	~CreateSceneEntityObjectEvent();

	static CreateSceneEntityObjectEvent * create();

public:
	void setResource(const std::string& mesh, const std::string& mat, uint light, uint particle, int cut);
	void setResource(uint id);

public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );
	void RButtonDown( UINT nFlags, CPoint point, bool & processed );
	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );
	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:
	void doFinish();

protected:
	SceneEntityObject*	mSceneEntityObject;

	uint				mEntityObjId;
	std::string			mMeshName;
	std::string			mMatName;
	uint				mLight;
	uint				mParticle;
	int					mCutDistanceLevel;
};


class SceneObjectTransform : public MfcInputEvent, public SceneInfo::Listener
{
public:
	explicit SceneObjectTransform();
	~SceneObjectTransform();

	static SceneObjectTransform * create();

public:
	enum TransMode {
		SELECT,
		MOVE,
		ROTATION,
		SCALE,
	};

	void initTransMode(TransMode mode);
	void changeTransMode(TransMode mode);

public:
	void LButtonDown( UINT nFlags, CPoint point, bool & processed );
	void LButtonUp( UINT nFlags, CPoint point, bool & processed );
	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );
	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

	void onDestroySceneObject(uint id);

protected:
	void changeScenObject(SceneObject* newObj);

	void moveSecneObject(CPoint delta, CPoint point);
	void rotationSceneObject(CPoint delta, CPoint point);
	void scaleSceneObject(CPoint delta, CPoint point);

	void delSelectObject();

	void delOperate(SceneObject* obj);

protected:
	TransMode			mCurrTransMode;
	SceneObject*		mCurrSceneObject;
	bool				mHitObject;			//左键点击时, 有按到
	Ogre::Vector3		mOffsetObject;
	float				mRotReferValue;		//旋转参考值

	uint				mOrgId;
	Ogre::Vector3		mOrgPos;
	Ogre::Quaternion	mOrgRot;
	Ogre::Vector3		mOrgScale;

};


#endif
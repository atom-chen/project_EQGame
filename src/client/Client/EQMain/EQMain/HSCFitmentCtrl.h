/******************************************************************************
* 作者: Y3
* 时间: 2009-01-05
* 描述: 房屋部件家装控制
******************************************************************************/
#ifndef HSCFitmentCtrl_H
#define HSCFitmentCtrl_H

#include "EQMainPrerequisites.h"

#include "HouseSceneComponent.h"
#include "HouseComponentGridGraphics.h"
#include "HouseSceneGrid.h"



/** 房屋部件家装控制
@remarks 不提供删除mComp操作, 是因为这个类不应该有这个行为
		客户端也需要家装控制, 所以必须有共用的一个家装控制类
*/
class HSCFitmentCtrl
{
public:
	enum PutMode 
	{
		pmEditor = 0,	// 编辑器模式
		pmPlayer,		// 玩家模式
	};
public:
	/// 连接部件
	void attach(HouseSceneComponent* hsc);

	/// 解除连接
	HouseSceneComponent* detach();

	/// 获取连接的部件
	HouseSceneComponent* getAttach() { return mComp; }

	/// 执行
	void exec(const Ogre::Ray& ray);

	/// 开始旋转
	void rotStart();

	/// 旋转结束
	void rotEnd();

	/// 旋转角度
	void rotate(float degree);

	/// 设置摆放模式
	void setPutMode( PutMode val ) { mPutMode = val; }
	PutMode getPutMode() const { return mPutMode; }

	Ogre::SceneNode* getSceneNode() { return mSceneNode; }
public:
	HSCFitmentCtrl();
	~HSCFitmentCtrl();
	
	class Listener
	{
	public:
		virtual ~Listener() {}

		virtual void notifyCanLay( HouseSceneGrid * grid , int face, int height, HouseSceneComponent * compRep ) = 0; 
		virtual void notifyForbidLay(HouseReturnValue hrv) {}
		virtual void notifyFaceChange(int face) {}
	};

	void setListener( Listener* l ) { mListener = l; }

private:
	HSCFitmentCtrl( const HSCFitmentCtrl & ); //no copy
	HSCFitmentCtrl & operator = ( const HSCFitmentCtrl & );

	void enterScene();
	void leaveScene();

	bool calcLay(HouseSceneGrid* grid);

private:
	bool					mRoting;
	int						mFace;
	/// 是否玩家模式, 玩家模式下不能摆放门窗，只能替换门窗
	PutMode					mPutMode;

	HouseSceneComponent *	mComp;
	Ogre::SceneNode *		mSceneNode;
	HouseSceneGrid *		mLastOperateGrid;
	Listener *				mListener;

	HouseComponentGridGraphics	mCompGridGraphics;
};


class HSCFitmentHelper
{
public:
	// 取得资源名称
	static std::string fetchResName();
	// 取得错误讯息
	static std::string getFitmentErrorStr( int err );
};

#endif

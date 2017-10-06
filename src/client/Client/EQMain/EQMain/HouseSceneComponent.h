/******************************************************************************
* 作者: Y3
* 时间: 2009-11-27
* 描述: 房屋部件
******************************************************************************/
#ifndef HouseSceneComponent_H
#define HouseSceneComponent_H

#include "EQMainPrerequisites.h"

#include "ModelGcs.h"
#include "HouseTable.h"
#include "HouseSceneDef.h"
#include "AABBBox.h"
#include "PickObjectProxy.h"

class ComponenetHouseRes;
class HouseSceneGrid;
class HouseSceneFloor;
class HouseSceneComponent;
class ZoneLight;


/** 房屋部件
@remarks 涉及到具体的实体, 是房屋的图形表现. 目前还缺少对粘滞在鼠标上的事件包装
*/
class HouseSceneComponent : public PickObject
{
	friend class HSCLayLogic;
public:
	/// 计算观察模式对物件的影响
	void calcWatchMode( HouseWatchMode mode );

	/// 摆放
	void lay( HouseSceneGrid * grid, int face, int height );

	/// 拿起
	void take();

	/// 获取朝向
	int getFace() const { return mFace; }

	/// 获取高度
	int getHeight() const { return mBottom; }

	/// 是否放置在网格上
	bool isLayed() const { return getAttachGrid() != NULL; }

	HouseSceneGrid * getAttachGrid() { return mHoldGrids[mHoldBox.linearIndex(0,0)]; }
	const HouseSceneGrid * getAttachGrid() const { return mHoldGrids[mHoldBox.linearIndex(0,0)]; }
	/// 获取占据的网格数组
	const std::vector<HouseSceneGrid *> & getHoldGrids() const { return mHoldGrids; }

	/// 是否靠墙物件
	bool isLayWallObject() const;

	/// 是否门窗
	bool isDoorOrWin() const;

	/// 根据坐标点转化格子数组
	static void convertGrids( HouseCompPropRow::GridPosArray & arr, int face, int gridX, int gridZ );
public:
	/// 不要直接调用这个函数, 请用HSCManager来创建
	HouseSceneComponent(uint uid);
	~HouseSceneComponent();

	const std::string& getClassName();

	/// 分析资源
	void parseRes(ComponenetHouseRes* res, const std::string& addPath = "");

	/// 分析资源
	void parseRes( uint compId );

	/// 分析资源
	void parseRes( const HouseCompRow * compRow, const HouseCompPropRow * propRow );

	/// 写入资源 
	void writeRes(ComponenetHouseRes * res);
	
	/// 加载
	void load();

	/// 获取唯一id
	uint getUid() const { return mUid; }

	/// 获取模型.
	ModelGcs* getModel() { return mModel; }
	const ModelGcs* getModel() const { return mModel; }

	/// 获取挂接灯光
	std::vector<ZoneLight*>& getLight() { return mLights; }
	void turnOnLight();
	void turnOffLight();

	int getFloorNO(){return mFlr;}
	uint getRoomNO(){return mRoom;}
	
public:
	bool isFromFrame() const { return mIsFromFrame; }

	/// 框架属性
	const std::string & getFrmMesh() const { return mFrmMesh; }
	const Ogre::Vector3 & getFrmPos() const { return mFrmPos; }
	const Ogre::Quaternion & getFrmRot() const { return mFrmRot; }
	const Ogre::Vector3 & getFrmScale() const { return mFrmScale; }
	const HouseCompPropRow * getHouseCompPropRow() const {	return mHouseCompPropRow; }
	const HouseCompRow * getHouseCompRow() const {	return mHouseCompRow; }

	/// 底部(y方向)
	int getBottom() const;

	/// 顶部(y方向) 
	int getTop() const;

	const Ogre::Vector3 & getPosition() const;
	const Ogre::Quaternion & getOrientation() const;

	/// 设置位置
	void setPosition( const Ogre::Vector3 & pos );

	/// 设置旋转
	void setOrientation( const Ogre::Quaternion & rot );

	/// 设置父节点
	void setParentNode( Ogre::SceneNode * parentNode );

	/// 获取资源名字
	const std::string & getResName() const { return mResName; }

	/// 设置资源名
	void setResName(const std::string & n) { mResName = n; }

	/// 计算使用网格
	std::set<const HouseSceneGrid*>& calcUseGrids(Ogre::uchar face, uint range);

	/// 计算家具交互信息
	std::vector<int>& calcInteractMorph(int pos, int flag, int user_count);

protected:
	void setVisible(bool vi);

	void _removeFromScene();

	/// 离开场景
	void unLay();
	void _updatePosAndRot();

	/// 计算占据网格
	std::vector<const HouseSceneGrid *> _calcHoldGrids( const HouseSceneGrid * attachGrid, int face ) const;

	bool _checkPassLights(bool& forward_indoor);

	void createSheetShadow();
	void destroySheetShadow();

private:
	HouseSceneComponent();

	void _initProperty();

protected: //属性
	/// 名字, 在资源导出时使用.用来辨别资源的唯一性, 以后,服务器交互可能需要这个信息
	std::string				mResName;

	/// 是否是框架
	bool					mIsFromFrame;

	/// 类型对应的表格数据
	const HouseCompPropRow * mHouseCompPropRow;

	/// 朝向
	int						mFace;

protected: //框架物件的特有属性集
	/// 模型名字
	std::string				mFrmMesh;

	/// 初始坐标
	Ogre::Vector3			mFrmPos;

	/// 初始化旋转
	Ogre::Quaternion		mFrmRot;

	/// 初始化缩放
	Ogre::Vector3			mFrmScale;

protected: //房屋部件表的特有属性集
	/// 房屋部件表数据
	const HouseCompRow *	mHouseCompRow;

protected:
	//如果已经摆放, 则放在哪些网格之上.
	//如果是放在别的物件上面, 则指定对应的物件

protected:
	/// 唯一ID
	uint					mUid;

	/// 是否加载
	bool					mLoaded;

	/// 占据格的box
	Int2AABBBox				mHoldBox;

	/// 占据网格数组
	std::vector<HouseSceneGrid *>	mHoldGrids;

	/// 使用网格
	std::set<const HouseSceneGrid *> mUseGrids;

	/// 家具交互信息
	std::vector<int>		mInteractMorph;

	///// 连接的网格
	//HouseSceneGrid*			mAttachGrid;

	/// 附加的路径
	std::string				mAddPath;

protected:
	/// 缓存的位置信息
	Ogre::Vector3			mPos;
	Ogre::Quaternion		mRot;

	/// 对应的模型实例
	ModelGcs *				mModel;

	/// 假阴影
	Ogre::ManualObject*		mSheetShadow;
	Ogre::SceneNode*		mSheetShadowNode;

	/// 选取代理
	PickObjectProxy			mProxy;

protected:
	/// 底部的高度 
	int						mBottom;

protected:
	/// 物件占据的区域
	std::vector<uint>		mHoldZone;
	/// 携带光源
	std::vector<ZoneLight*>	mLights;
protected:
	/// 所在楼层
	int						mFlr;
	/// 所在房间
	uint					mRoom;
};



/** 房屋部件摆放逻辑
*/
class HSCLayLogic
{
public:
	/// 在某个格子上找可以替换的物件
	static HouseSceneComponent * findCanReplaceComponent( const HouseSceneComponent * pThis, const HouseSceneGrid * grid );

	/// 判断一个物件是否能替换另一个物件
	static bool canReplaceComponent( const HouseSceneComponent * pThis, const HouseSceneComponent * dst );

	/// 能否摆放
	static HouseReturnValue canLay( const HouseSceneComponent * pThis, const HouseSceneGrid * grid, int face, int & height );

	/// 自动靠墙
	static HouseReturnValue canAutoLayWall( const HouseSceneComponent * pThis, const HouseSceneGrid * grid, int & face, int & height );

	/// 能否被拿起
	static HouseReturnValue canTake( const HouseSceneComponent * pThis );

	static std::pair<bool, const HouseSceneComponent *> getFirstCanLayComponent( const HouseSceneComponent * pThis, const HouseSceneGrid * grid );
};


#endif

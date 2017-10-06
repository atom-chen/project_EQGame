/******************************************************************************
* 作者: Y3
* 时间: 2009-11-27
* 描述: 房屋. 
******************************************************************************/
#ifndef HouseScene_H
#define HouseScene_H

#include "EQMainPrerequisites.h"

#include "HouseSceneDef.h"
#include "HouseSceneFloor.h"
#include "HouseSceneComponent.h"
#include "HouseRes.h"
#include "HSCFitmentCtrl.h"



class HouseSceneSerializer;

/** 房屋
@remarks 
	房屋的场景管理, 与编辑器和以后游戏中的家装模式有紧密相关. 这个类, 及其附属类,
	层次逻辑层次目前定位在纯粹逻辑层和图形表现层之间, 必然与这2者有复杂的联系.
	这样设计未必很合适, 但实在没有更好的想法.
@par
	房屋场景的基本结构是: 房屋 (包含) 楼层 (包含) 格子 (包含) 部件
@par
	目前的逻辑是一个场景只能有一个HouseScene. 换句话, 就是HouseScene就是游戏场景.
*/
class HouseScene
{	
	class HouseScenePairGrid : public std::pair<const HouseSceneGrid *, const HouseSceneGrid *>
	{
	public:
		HouseScenePairGrid( HouseSceneGrid * a, HouseSceneGrid * b )
			: std::pair<const HouseSceneGrid *, const HouseSceneGrid *>( a, b )
		{
			if ( this->first > this->second )
			{
				std::swap( this->first, this->second );
			}
		}
	};
public:
	friend class HouseSceneSerializer;
	typedef HouseScenePairGrid PairGrid;
	typedef std::set<PairGrid> PairGridList;
	typedef std::map<PairGrid, Ogre::ManualObject *> PairGridObjectList;

public:	//常用的方法
	HouseScene(Ogre::Camera* cam);
	~HouseScene();

	/// 创建房屋
	void buildHouse();

	/// 是否已经加载了房屋
	bool isLoadHouse() { return mHouseLoaded; }

	/// 创建基础层(花园层)
	//TODO:

	/// 获取当前层
	HouseSceneFloor* getWalkViewFloor() { return mWalkViewFloor; }
	/// 获取最低层
	HouseSceneFloor* getRootHouseFloor() { return mRootHouseFloor; }

	/// 获取层
	HouseSceneFloor* getFloorByNum(int n);
 
	/** 获取ray网格. 
	@param ray 输入射线, ray方向必须是朝下, 即必须是-y才能正常工作
	@param gr 返回相交的网格
	@param worldCrossingPoint 返回相交的世界坐标交点
	@param allHouse 强制整个房子. true, 不管对应楼层显示是否隐藏都会对整个房子检测检测; false, 只从当前楼层开始检测
	*/
	bool rayGrid( const Ogre::Ray& ray, HouseSceneGrid *& gr, Ogre::Vector3 & worldCrossingPoint, bool allHouse = false );

	/** 获取坐标对应的网格
	@param pos 世界坐标
	@param YBias Y轴的偏差. 点经常不会正好在面上, 而是在一个体积里, 所以需要这个值.
	@param NeYBias -Y轴的偏差.
	@return 返回网格, 如果没找到, 返回空
	*/
	HouseSceneGrid* gridByPosition(const Ogre::Vector3& pos, float YBias = 0.5f, float NeYBias = 0.5f);

	/** 获取当前楼层的交叉点 
	*/
	void getXZ( int & x, int & z, const Ogre::Ray & ray );

	HouseSceneGrid * getGrid( int flrNO, int x, int z );

	/// 获取家装控制类的应用
	HSCFitmentCtrl& getFitmentCtrl() { return mFitmentCtrl; }

	/// 变更观察模式
	void changeWatchMode(HouseWatchMode mode);

	/// 获取观察模式
	HouseWatchMode getWatchMode() const { return mWatchMode; }

	/// 获取对应楼层的观察模式
	HouseWatchMode getWatchModeByFloorNO(int flrNO) const;

	/// 看上一层 
	bool upView();

	/// 看下一层
	bool downView();

	/// 改变网格显示模式
	void changeGridDisplayMode(GridDisplayMode mode);

	GridDisplayMode getGridDisplayMode() const { return mGridDisplayMode; }


	Ogre::SceneNode* getSceneNode() { return mSceneNode; }

	Ogre::Camera * getCamera() { return mCamera; }

	void update(float delta);

	void turnAllLight(bool on);

public:
	/** 是否能转换为资源
	@remarks 条件 1.房屋部件要有名字, 且不能同名
	@param reason 返回失败的原因
	*/
	bool canConvertToResource(std::string& reason);

	/** 转换成资源
	@remarks 这只是个辅助资源, 他会脱离HouseResManager的管理.
	*/
	HouseResPtr convertToResource();

	const std::map<int, HouseSceneFloor*> & getFloors() const { return mFloors; }
	/// 增加部件到场景, 内部函数
	void _attachComponent(HouseSceneComponent* comp);

	/// 解除部件连接, 内部函数
	void _detachComponent(uint id);

	/// 获取场景物件列表
	const std::map<uint, HouseSceneComponent*>& _getComponentMap() const {return mComponentMap;}

	/// 取得梯子列表
	const PairGridList & getPairGridList() const { return mPairGridList; }

	/// 增加梯子
	void addStair( HouseSceneGrid * a, HouseSceneGrid * b );

	/// 删除梯子
	void removeStair( HouseSceneGrid * a, HouseSceneGrid * b );

	/// 设置辅助平面格子数(半径)
	void setFloorAssistGridNum( int n );

	/// 取得辅助格子数 
	int getFloorAssistGridNum() const { return mFloorAssistGridNum; }

	/// 显示辅助平面
	void showAssistPlane( bool bShow );

private:
	/// 楼梯连通信息的图形表现
	void addStairGraphics( HouseSceneGrid * a, HouseSceneGrid * b );
	void removeStairGraphics( HouseSceneGrid * a, HouseSceneGrid * b );
	void showStairGraphics( bool bVal );
	void _destroyManualObject( Ogre::ManualObject * obj );
	/// 创建辅助平面
	void _createAssistPlane( bool bVisible );
	/// 更新辅助平面高度
	void _updateAssistPlaneHeight();
protected:
	void setWalkViewFloor( HouseSceneFloor * flr ) { mWalkViewFloor = flr; }
	void setWatchMode( HouseWatchMode mode ) { mWatchMode = mode; }
	void _buildFloors(HouseResPtr house);
	void _buildComps(HouseResPtr house);
	void _initGraphics();
	void _destroyGraphics();
private:
	/// 不允许这种方式
	HouseScene();

protected: 
	/// 楼层的数据
	std::map<int, HouseSceneFloor *> mFloors;

	/// 房屋楼层最底层. 是否可以为空, 待定
	HouseSceneFloor *				mRootHouseFloor;

	/// 当前在那一层显示, 其实可以考虑让这个值一直有效. 可以是庄园层
	HouseSceneFloor *				mWalkViewFloor;

	/// 场景拥有的物件. 物件的管理主要有HouseSceneGrid处理. 这里是为了说明场景有这么多物件
	std::map<uint, HouseSceneComponent *> mComponentMap;

	/// 连通信息数据
	PairGridList					mPairGridList;

	/// 家装控制
	HSCFitmentCtrl					mFitmentCtrl;
	/// 观察模式
	HouseWatchMode					mWatchMode;
	/// 网格显示模式
	GridDisplayMode					mGridDisplayMode;

	/// 材质资源的路径
	std::string						mMatPathBak;

	/// 是否已经加载了房屋资源
	bool							mHouseLoaded;
	/// 房屋资源的名字
	std::string						mHouseResName;

protected: // 图形
	Ogre::SceneNode *				mSceneNode;
	Ogre::Camera *					mCamera;	

	/// 地图上的连接点
	PairGridObjectList				mPairGridObjectList;

	/// 辅助平面面板
	Ogre::ManualObject *			mFloorAssistPlane;
	int								mFloorAssistPlaneHeight;
	/// 多少个格子
	int								mFloorAssistGridNum;

};

#endif
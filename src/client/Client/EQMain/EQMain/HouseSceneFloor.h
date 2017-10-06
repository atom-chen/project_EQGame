/******************************************************************************
* 作者: Y3
* 时间: 2009-11-27
* 描述: 房屋楼层
******************************************************************************/
#ifndef HouseSceneFloor_H
#define HouseSceneFloor_H

#include "EQMainPrerequisites.h"

#include "HouseSceneGrid.h"
#include "CompareVectorTemplate.h"

/** 房屋楼层
@remarks 以后如果有带花园, 估计也归纳为一层. (所以设计时,楼层最好不要用0,1,2这样标识)
	以后楼层可能还会带房间信息.
*/
class HouseSceneFloor
{
	friend class HouseScene;
public:
	typedef CompareVector2Template<int> GridNo;
	typedef std::map<GridNo, HouseSceneGrid*> GridMap;
public:
	/// 获取当前层高度
	int getHeight() const;

	/// 调整楼层高度. 只有没格子的时候才能设置楼层高度. 否则异常
	void modifyFloorHeight( float h );

	/// 获取当前的楼层编号
	int getFloorNO() const { return mFloorNO; }

	/// 计算网格显示模式
	void calcGridDisplayMode(GridDisplayMode mode);

	/// 获取或创建房屋的根格子, 这个格子是为房屋框架准备的.
	const HouseSceneGrid * getRootGrid() const;
	HouseSceneGrid * getRootGrid();

	/// 创建格子
	HouseSceneGrid* createGrid( int x, int z, ushort typeID, ushort roomID=0 );

	/// 获取格子
	HouseSceneGrid* getGrid( int x, int z );
	const HouseSceneGrid* getGrid( int x, int z ) const;

	/// 是否有对应的格子
	bool hasGrid( int x, int z ) const;

	/// 是否有网格
	bool hasGrids() const;

	/// 删除格子
	void destroyGrid( int x, int z );

	const GridMap & getGridMap() const { return mGridMap; }

	/// 格子被增删或发生改变
	void notifyGridChanged();

	/// 房间编号被改了
	void notifyRoomIDChanged();
public: // 图形
	//Ogre::SceneNode * getSceneNode() { return mSceneNode; }
	/// 计算观察模式, 可能会修改
	void calcWatchMode( HouseWatchMode mode );

	/// 获取楼层高度(离地面0高度)
	int getFloorHeight() const { return mFloorHeight; }
public:
	HouseSceneFloor(HouseScene* scene);
	~HouseSceneFloor();

	/// 获取创建者
	HouseScene* getCreator() { return mCreator; }

	/// 设置上一层
	void setUpFloor(HouseSceneFloor* up);

	/// 获取上一层
	HouseSceneFloor* getUpFloor() { return mUpFloor; }
	const HouseSceneFloor* getUpFloor() const { return mUpFloor; }

	/// 设置下一层
	void setDownFloor(HouseSceneFloor* down);

	/// 设置下一层
	HouseSceneFloor* getDownFloor() { return mDownFloor; }
	const HouseSceneFloor* getDownFloor() const { return mDownFloor; }

	/// 只用于编辑器，显示网格所在房间ID
	void filterByRoomID( int roomid );
	void showRoomIDPlane( bool bShow );
private:
	void _showGridTypePlane( bool bShow );

	void _showWallPlane( bool bShow );

	void _showHoldGridPlane( bool bShow, bool bFitmentMode = false );

	void _showRoomIDPlane( bool bShow, bool bCareID, int filterID );
private:
	HouseSceneFloor();

protected:
	HouseScene*					mCreator;

	/// 根格子, 每层至少有这样1个根格子. 这目前主要为从max导出的房屋框架准备的准备的. 如果是花园层应该没这个信息
	HouseSceneGrid*				mRootGrid;

	/// 格子表 这里需要优化,到时建立二维数组
	GridMap						mGridMap;

	/// 楼层高度
	int							mFloorHeight;

	/// 当前第几层层数
	int							mFloorNO;

	/// 上一层
	HouseSceneFloor*			mUpFloor;

	/// 下一层
	HouseSceneFloor*			mDownFloor;
protected: // 图形
	/// 网格类型面板
	Ogre::ManualObject *		mGridTypePlane;

	/// 墙体面板数组
	Ogre::ManualObject *		mWallPlane;

	/// 网格模型
	Ogre::ManualObject *		mHoldGridPlane;

	/// 房间号数字
	Ogre::ManualObject *		mRoomIDPlane;

	/// 
	Ogre::SceneNode *			mSceneNode;

/// 楼层部件管理
public:
	/// 楼层房间内的所有物件，不包括灯光，不适用于室外
	struct RoomComponent
	{
		RoomComponent():floor(NULL),tcx(NULL){}
		HouseSceneComponent* floor;
		std::vector<HouseSceneComponent*> wall;
		std::vector<HouseSceneComponent*> tcx;
		std::vector<HouseSceneComponent*> sta;
		std::vector<HouseSceneComponent*> comp;
	};
	typedef std::map<uint, RoomComponent> RoomComponentMap;

	void insertRoomComponent(uint room, HouseSceneComponent* comp);
	void removeRoomComponent(uint room, HouseSceneComponent* comp);

	void changeRoomFloorMaterial(uint room, std::string& floormat);
	void changeRoomWallMaterial(uint room, std::string& wallmat);
	void changeRoomTcxMaterial(uint room, std::string& tcxmat);

	// 获取房屋部件和对应的材料 (2011/7/25 add by hezhr)
	RoomComponent* getRoomComponent(uint room);
	std::string getRoomFloorMaterial(uint room);
	std::string getRoomWallMaterial(uint room);
	std::string getRoomTcxMaterial(uint room);

private:
	RoomComponentMap mRoomComponentMap;
};


#endif

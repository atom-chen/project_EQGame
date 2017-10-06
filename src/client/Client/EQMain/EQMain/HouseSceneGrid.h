/******************************************************************************
* 作者: Y3
* 时间: 2009-11-27
* 描述: 房屋的格子信息
******************************************************************************/
#ifndef HouseSceneGrid_H
#define HouseSceneGrid_H

#include "EQMainPrerequisites.h"

#include "HouseSceneComponent.h"

struct HouseGridTypeRow;


/** 房屋的格子
@remarks 以后如果有带花园, 花园的格子可能还需要扩展, 或者不适用了. 
@par 可以考虑先把格子上的物件移开, 才能修改属性格子
*/
class HouseSceneGrid
{
public:
	typedef std::list<HouseSceneComponent *> ComponentList;
public:
	/// 
	bool hasComponents() const { return !mHouseSceneComponentList.empty(); }

	void attachComponent( HouseSceneComponent * com );
	void detachComponent( HouseSceneComponent * com );

	/// 是否是邻居网格, 同时返回朝向
	bool isNeighborGrid( const HouseSceneGrid * grid, int & face ) const;

	/// 设置是否框架
	void frameGrid( bool b );

	/// 是否是框架格子
	bool isFrameGrid() const { return mIsFrameGrid; }

	/// 获取网格编号
	void getGridNo( int& x, int& z ) const { x = mGridX; z = mGridZ; }
	int getGridX() const { return mGridX; }
	int getGridZ() const { return mGridZ; }

	/// 设置网格编号
	void setGridNo( int x, int z );

	/// 设置格子类型, 只能设置一次
	void setGridType( ushort typeID );

	/** 设置哪面是墙, 这是双方的行为, 返回另外一个影响的网格, 这个网格必须有.
	remarks face 0为x正方向. 逆时针方向存取,既, 0,x  1,-z  2,-x  3,z
	*/
	HouseSceneGrid* setWallFace( int face, bool wall );

	/// 获取墙体情况
	bool getWallFace( int face ) const { return mWallFace[face]; }

	/// 获取朝向的邻居网格坐标
	void getFaceGridNo( int face, int& x, int& z ) const;

	bool hasWallFace() const;

	/// 设为阻挡
	void setBlocked( bool bVal );

	/// 判断是否阻挡
	bool isBlocked() const { return mBlocked; }

	const HouseGridTypeRow * getHouseGridTypeRow() const { return mHouseGridTypeRow; }

	void setRoomID(int id) { mRoomID = id; }
	Ogre::uint getRoomID() const { return mRoomID; }
	Ogre::SceneNode* getSceneNode();	
	const Ogre::SceneNode* getSceneNode() const;	

	/// 计算观察模式, 可能会修改
	void calcWatchMode( HouseWatchMode mode );

protected:
	/// 获取反方向
	static int negativeFace( int face );
public:
	HouseSceneGrid(HouseSceneFloor* floor);
	~HouseSceneGrid();

	HouseSceneFloor* getCreator() { return mCreator; }
	const HouseSceneFloor* getCreator() const { return mCreator; }


	const ComponentList & getHouseSceneComponentList() const { return mHouseSceneComponentList; }
	//对应一个物件目前叠加了几个东西

	//获取当前格子直接放的物件列表
	////获取格子的所有物件列表, 这个方法会出问题
private:
	HouseSceneGrid();

	void _updatePosition();	
	void _exceptionIfHasComp();
protected:
	// 隶属哪个楼层
	HouseSceneFloor*	mCreator;

protected:	//格子属性
	/// 格子类型
	const HouseGridTypeRow * mHouseGridTypeRow;

	/// 是不是框架格子
	bool				mIsFrameGrid;

	/// 阻档属性
	bool				mBlocked;

	/// 格子编号
	int					mGridX;
	int					mGridZ;

	/// 哪面是墙体, 同时也是临近格子不可连通属性(目前是). 0为x正方向. 逆时针方向存取
	bool				mWallFace[WALL_FACE_COUNT];

protected:	//物件
	//可摆放属性-----------------------
	/// 已摆放的列表
	ComponentList		mHouseSceneComponentList;

	/// 网格所在房间编号
	Ogre::uint			mRoomID;
protected:	//图形相关
	/// 可以考虑优化为, 没有child时不加入HouseScene节点
	Ogre::SceneNode*	mSceneNode;
};


#endif

/******************************************************************************
* 作者: Y3
* 时间: 2009-11-20
* 描述: 子房屋资源
******************************************************************************/
#ifndef SubHouseRes_H
#define SubHouseRes_H

#include "EQMainPrerequisites.h"

class HouseRes;

/** 楼层的房屋资源
*/
class FloorHouseRes
{
public:
	/// 编号
	int					mNO;

	/// 楼上的楼层的编号, >255为无效值
	int					mUpFloorNO;

	/// 楼层的高度
	int					mHeight;


	/** 网格资源
	*/
	class Grid
	{
	public:
		/// 网格类型
		ushort			mType;

		///// 是不是框架网格
		//bool			mFrame;	//默认有

		/// 网格在楼层中的编号
		int				mX;
		int				mZ;

		/// 网格所属房间编号
		ushort			mRoom;

	public:
		Grid();
	};

	/// 网格列表
	std::vector<Grid>	mGrids;


	/** 墙体
	*/
	class Wall
	{
	public:
		/// 墙体对应在网格中的编号
		int				mX0;
		int				mZ0;
		int				mX1;
		int				mZ1;

	public:
		Wall();
	};

	/// 墙体列表
	std::vector<Wall>	mWalls;

public:
	FloorHouseRes();
};


/** 房屋部件资源
*/
class ComponenetHouseRes
{
public:
	/// 名字, 唯一标识
	std::string			mName;

	/// 是否是框架部件
	bool				mFrame;

	/// 框架部件类型
	ushort				mFrmCompType;

	/// 框架部件名字
	std::string			mFrmMesh;

	/// 框架部件的空间位置
	Ogre::Vector3		mFrmPos;
	Ogre::Quaternion	mFrmRot;
	Ogre::Vector3		mFrmScale;

	/// 普通部件对应HouseComp.xml表的id
	ulong				mCompId;

	/// 摆放的楼层id
	uint				mFlr;
	/// 摆放的房间id
	uint				mRoom;

	/// 摆放物件中心格子对应楼层格子的编号
	int					mX;
	int					mZ;

	/// 摆放物件的朝向
	int					mFace;

	/// 属于摆放的第几层, 用于确定摆放顺序. 0为最底层
	int					mLayer;

	/// 摆放底部的高度
	int					mBottom;
public:
	ComponenetHouseRes();
};

/** 连接格子
*/
class LinkGridRes
{
	class Grid
	{
	public:
		int mFloorNo;
		int mX;
		int mZ;
	};
public:
	Grid mStartGrid;
	Grid mEndGrid;
};

#endif




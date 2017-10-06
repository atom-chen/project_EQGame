/******************************************************************************
* 作者: Y3
* 时间: 2009-11-27
* 描述: 房屋定义部分
******************************************************************************/
#ifndef HouseSceneDef_H
#define HouseSceneDef_H

#include "EQMainPrerequisites.h"

const int HOUSE_DEF_FLOOR_HEGHT		= 280;					//楼层默认高度(厘米)

const int HOUSE_GRID_SIZE			= 80;					//房屋格子的边长(厘米)
const int HOUSE_GRID_HALF_SIZE		= 40;

const int HOUSE_BLOCKED_GRID_TYPE	= 144;					//阻挡格子 
const int HOUSE_VISIBLE_GRID_TYPE	= 127;					//可见格子 

const int WALL_FACE_COUNT			= 4;					//格子有几面墙
const int WALL_FACE_X				= 0;					//X墙
const int WALL_FACE_NX				= 2;					//NX墙
const int WALL_FACE_Z				= 3;					//Z墙
const int WALL_FACE_NZ				= 1;					//NZ墙

/// 房屋的观察模式
enum HouseWatchMode
{
	HW_MODE_HIDE	= 0,		// 隐藏, 没这种观察模式. 调试需要而已
	HW_MODE_1		= 1,		// 观察模式1, 无墙
	HW_MODE_2		= 2,		// 观察模式2, 半墙
	HW_MODE_3		= 3,		// 观察模式3, 全墙
	HW_MODE_4		= 4,		// 观察模式4, 屋顶
};

/// 房屋部件被观察模式
enum CompBeWatchMode
{
	CW_MODE_ROOF	= 0,		// 显示到屋顶
	CW_MODE_INWALL	= 1,		// 显示到内墙
	CW_MODE_OUTWALL	= 2,		// 显示到外墙
	CW_MODE_COMM	= 3,		// 普通物件,全部观察.
	CW_MODE_LIMIT	= 4,		// 上限
};

/// 网格显示方式
enum GridDisplayMode
{
	GD_MODE_HIDE	= 0,		// 不显示, 隐藏用, 没有这种方式
	GD_MODE_GAME	= 1,		// 游戏中非家装模式也就是这种方式. 当然目前的效果和GD_MODE_HIDE一样
	GD_MODE_FITMENT	= 2,		// 只显示有特定属性的网格边框, 在游戏家装模式为这种方式
	GD_MODE_STYLE	= 3,		// 只用于编辑器 显示网格的边框和类型
	GD_MODE_CONNECTEDNESS	= 4,// 只用于编辑器 显示网格的边框和类型和房间号和连通性
};



/// 房屋系统的一些方法返回值, 根据这些值, 界面自行显示提示
enum HouseReturnValue
{
	HRV_FAIL		= -1,		// 失败
	HRV_OK			= 0,		// 成功

	//摆放网格相关
	HRV_LAYED		= 1,		// 已经摆放
	HRV_NO_GRID_LAY	= 2,		// 没有网格放置
	HRV_WALL_STOP	= 3,		// 有墙体阻挡
	HRV_NEAR_WALL	= 4,		// 必须靠墙
	HRV_NO_LAY_SPACE= 5,		// 没有摆放空间
	HRV_NO_AUTO_LAYWALL	= 6,	// 不能自动靠墙

	//拿取相关
	HRV_NO_LAYED	= 20,		// 没摆放
	HRV_NOTAKE_FRAME= 21,		// 框架部件不能拿取
	HRV_NOTAKE		= 21,		// 无法拿取

	//摆放属性相关
	HRV_HAS_COMP_LAY= 40,		// 有物件摆放在上面
	HRV_NO_MATCH_LAYTYPE = 41,	// 放置属性不符
};



#endif

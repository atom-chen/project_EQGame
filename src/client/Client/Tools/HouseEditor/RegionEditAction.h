/******************************************************************************
* 作者: Y3
* 时间: 2010-9-30
* 描述: 区域编辑. 来自TD
******************************************************************************/
#ifndef __REGIONEDITACTION_H__
#define __REGIONEDITACTION_H__

#include "UnreachRegion.h"
#include "Action.h"

#define		REGION_VER0			"[REGION_00]"
#define		CUR_REGION_VER		REGION_VER0

#define		FUNCTION_KEY        0X00000000
#define		FUNC_KEY_SHIFT		0X00000001
#define		FUNC_KEY_CTRL		0X00000002
#define		FUNC_KEY_ALT		0X00000004

enum OPERATOR_TYPE
{
	NO_OPERATOR   = 0,
	ADD_NEW_POINT,
	MODITY_POINT_POSITION,
};

typedef std::map<unsigned long, CUnreachRegion*> UNREACHREGIONMAP;

class RegionEditManager
{
public:
	class Listener
	{
	public:
		//不可行走区域回掉函数
		virtual void onRegionAddNewPoint(unsigned long ulRegionId,unsigned long ulPointId, Ogre::Vector3 position) {}

		// 删除一个点的回调函数
		virtual void onRegionDelPoint(unsigned long ulRegionId,unsigned long ulPointId) {}

		// 选中一个区域
		virtual void onRegionSel(unsigned long ulRegionId) {}
	};
public:

	// 不可行走区域列表
	UNREACHREGIONMAP m_UnreachRegionMap;

	// 是否显示场景.
	bool  m_bIsShow;

	Listener* mListener;

public:
	RegionEditManager();
	~RegionEditManager();

	void setListener(Listener* l) {mListener = l;}

	////////////////////////////////////////////////////////////////////////////////////
	//
	// 存盘和读盘操作
	//

	// 从区域读取文件
	unsigned long ReadRegionFromFileVer0(std::string strFileName);

	// 把区域存储到文件
	unsigned long SaveRegionToFileVer0(std::string strFileName);

	// 从区域读取文件
	unsigned long ReadRegionFromFile(std::string strFileName);

	// 把区域存储到文件
	unsigned long SaveRegionToFile(std::string strFileName);

public:
	////////////////////////////////////////////////////////////////////////////////////
	//
	// 编辑不可行走区域的数据
	//
	Ogre::SceneNode*	m_pSceneNode;			// 用来显示区域编辑的总的节点.

	unsigned long m_ulCurMaxId;					// 当前最大的区域ID
	unsigned long m_ulCurEditRegionId;			// 当前正在编辑的区域的id
	unsigned long m_ulCurEditPointId;			// 当前正在编辑的点的id
	unsigned long m_ulCurEditLineId;			// 当前正在编辑的线的id

	// 得到当前正在编辑的区域的id
	int GetCurEditRegionId()
	{
		return m_ulCurEditRegionId;
	}

	//设置当前区域id
	void SetCurEditRegionId(unsigned long ulRegionId)
	{
		m_ulCurEditRegionId = ulRegionId;
	}

	// 得到当前正在编辑的点的id
	int GetCurEditPointId()
	{
		return m_ulCurEditPointId;
	}
	//设置当前点id
	void SetCurEditPointId(unsigned long ulPointId)
	{
		m_ulCurEditPointId = ulPointId;
	}

	// 得到当前正在编辑的线的id
	int GetCurEditLineId()
	{
		return m_ulCurEditLineId;
	}//

	//设置当前线id
	void SetCurEditLineId(unsigned long ulLineId)
	{
		m_ulCurEditLineId = ulLineId;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// 区域操作
	//

	// 添加一个新的区域
	unsigned long AddNewRegion();

	// 通过指定的id， 删除一个区域
	unsigned long DelRegion(unsigned long ulId);

	// 清除所有区域数据.
	void ClearRegionData();

	// 根据指定的id， 得到区域
	CUnreachRegion* GetRegionById(unsigned long ulId);

	// 选择一个区域
	void SelRegion(unsigned long ulId, bool bIsChangeCamera = true);

	// 不选择一个区域.
	void UnSelRegion(unsigned long ulId);

	// 隐藏一个区域的面积
	void HideRegionArea(unsigned long ulRegionId, bool bHide);

	// 当前的区域是否合法
	bool IsRegionValidate(unsigned long ulRegionId);

	// 重新创建显示的面积区域
	void ReshapeArea(unsigned long ulRegionId);

	UNREACHREGIONMAP* GetUnreachRegionMap() { return &m_UnreachRegionMap; }

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// 点操作
	//

	// 当前操作之前的点的位置.
	Ogre::Vector3 m_preDoPointPos;

	// 当前操作之前的点的位置.
	Ogre::Vector3 m_curPointPos;

	// 在当前编辑的区域中添加一个新的点
	unsigned long AddNewPoint(Ogre::Vector3 position);

	// 指定区域id， 指定点的id， 指定位置， 添加一个小球.   
	void AddNewPoint(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position);

	// 指定区域id， 指定点的id， 指定位置， 删除一个小球. 
	void DelPoint(unsigned long ulRegionId, unsigned long ulPointId);

	// 操作点 
	bool EditPoint(const Ogre::Vector2& pt);

	// 选择一个点.
	void SelPoint(unsigned long ulPointId);

	//void DelPoint(unsigned long ulPointId);

	// 改变当前编辑的点的位置, 参数是鼠标坐标
	void ChangeCurEditPointPos(const Ogre::Vector2& pt);

	// 改变指定区域中指定的点的位置
	void ChangeCurEditPointPos(unsigned long ulRegionId, unsigned long ulPointId, Ogre::Vector3 position);

	// 记录当前操作前一次点的位置.
	void RecordPreDoPointPos();

public:

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// 场景的操作
	//

	// 前一次鼠标操作类型.

	//int m_iPreOperatorType;

	// 初始化区域编辑需要的一些数据
	void InitRegionEdit();

	// 设置其他编辑的鼠标点小球的材质
	void SetOldHitPointMaterial();

	// 创建区域编辑的场景节点
	void CreateSceneNode();

	// 隐藏区域编辑场景节点
	void HideScene(bool bHide);

	// 根据地形的高度改变编辑的区域。
	void SetRegionHeight();

	//
	bool m_bShowDeepBuf;

	// 改变没有深度缓冲区的材质
	void ChangeNoDeepMaterial();

public:

	// 摄像机的位置操作.
	void SetCamera(float fx, float fy);

};



class CRegionEditAction : public Action
{
public:
	static CRegionEditAction* create();

public:
	CRegionEditAction();
	const std::string& getName(void) const;


protected:


	// 鼠标操作的虚函数.
	void _onMove(const Ogre::Vector2& pt);
	void _onBegin(const Ogre::Vector2& pt);
	void _onDrag(const Ogre::Vector2& pt);
	void _onEnd(const Ogre::Vector2& pt, bool canceled);

	// 当前激活active操作.
	void _onActive(bool active);

	void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed );

protected:
	RegionEditManager*	m_Handle;

	// 前一次鼠标操作类型.

	int m_iPreOperatorType;

};


#endif
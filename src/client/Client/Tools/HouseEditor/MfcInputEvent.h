/******************************************************************************
* 作者: Y3
* 时间: 2009-12-10
* 描述: Mfc输入事件基类
******************************************************************************/
#ifndef MfcInputEvent_H
#define MfcInputEvent_H


/** 输入事件基类
*/
class MfcInputEvent
{
public:
	/** 集中放置所有事件的优先级，便于查看及调整
	*/
	enum EventPriority
	{
		epCameraFly			= 80,	// 相机
		epSetRoomID			= 50,	// 设置格子的房间ID
		epTestAutoFindPath	= 50,	// 测试自动寻路
		epCreateGrid		= 50,	// 增删格子
		epWallGrid			= 50,	// 增删墙
		epLinkGrid			= 50,	// 增删连通信息
		epBrowseGrid		= 50,	// 查看格子
		epBrowseComponent	= 50,	// 查看物件
		epEditFitment		= 50,	// 家装摆放编辑
		epTDAction			= 50,	// 来自td的事件优先级集合
		epSceneObject		= 50,	// 场景物件相关的事件优先级集合
		epNavMesh			= 50,	// 导航网格
		epOusideCut			= 50,	// 室外物件剔除
	};
public:
	MfcInputEvent( int priority  ) : mPriority(priority) {}
	virtual ~MfcInputEvent() {}

	/// 获取优先级
	int getPriority() const { return mPriority; }

	/// 是否是TD的事件
	virtual bool isTDAction() { return false; }

public: //事件相关
	/** 鼠标左键按下事件
	@param processed 这个操作之前是否已经被处理过
	@return 返回未被处理过
	*/
	virtual void LButtonDown( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void LButtonUp( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void RButtonDown( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void RButtonUp( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void MButtonDown( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void MButtonUp( UINT nFlags, CPoint point, bool & processed ) { }

	virtual void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed ) { }

	virtual void MouseWheel( UINT nFlags, short zDelta, CPoint pt, bool & processed ) { }

	virtual void KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed ) { }

	virtual void KeyUp( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed ) {  }
private:
	/// 优先级 初始化完后就不允许被修改
	int		mPriority;
};


#endif

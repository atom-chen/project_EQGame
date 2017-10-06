/******************************************************************************
* 作者: Mike
* 时间: 2010-7-05
* 描述: 输入事件基类
******************************************************************************/
#ifndef INPUTDEF_H
#define INPUTDEF_H

/** EventPriorityFlags
	1）集中放置所有事件的优先级
	2）标识前面处理过的事件
*/
#define	epfUI				(1 << 12)
#define	epfHotkey			(1 << 11)
#define	epfDragItem			(1 << 11)
#define	epfCameraFly		(1 << 10)
#define	epfFitEvent			(1 << 8)
#define	epfPick				(1 << 6)
#define	epfCursor			(1 << 4)
#define	epfMainplayer		(1 << 2)
#define	epfNone				0

/** 输入事件基类
*/
class EQInputEventListener
{
public:
public:
	EQInputEventListener( int priority  ) : mPriority(priority) 
	{}
	virtual ~EQInputEventListener() {}

	/// 获取优先级
	int getPriority() const { return mPriority; }

public: //事件相关
	/** 鼠标左键按下事件
	@param processed 这个操作之前是否已经被处理过
	@return 返回未被处理过
	*/
	virtual void LButtonDown( UINT& flags, POINT point ) { }
	virtual void LButtonUp	( UINT& flags, POINT point ) { }
	virtual void RButtonDown( UINT& flags, POINT point ) { }
	virtual void RButtonUp	( UINT& flags, POINT point ) { }
	virtual void MButtonDown( UINT& flags, POINT point ) { }
	virtual void MButtonUp	( UINT& flags, POINT point ) { }
	virtual void MouseMove	( UINT& flags, POINT delta, POINT point ) { }
	virtual void MouseWheel	( UINT& flags, POINT point, short zDelta ){ }
	virtual void KeyDown	( UINT& flags, UINT nChar, UINT nRepCnt ) { }
	virtual void KeyUp		( UINT& flags, UINT nChar, UINT nRepCnt ) { }

	virtual void Update		(float timElapsed) { }
private:
	/// 优先级 初始化完后就不允许被修改
	int		mPriority;
};


#endif

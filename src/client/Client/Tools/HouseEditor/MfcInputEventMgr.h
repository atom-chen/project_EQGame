/******************************************************************************
* 作者: Y3
* 时间: 2009-12-10
* 描述: Mfc输入事件管理器
******************************************************************************/
#ifndef MfcInputEventMgr_H
#define MfcInputEventMgr_H


#include "MfcInputEvent.h"


/** MFC输入事件管理器
*/
class MfcInputEventMgr : public Ogre::Singleton<MfcInputEventMgr>
{
public:
	typedef Ogre::SharedPtr<MfcInputEvent> TSharedEvent;
	typedef std::map<int, TSharedEvent> TEventList; 
public:
	MfcInputEventMgr();
	~MfcInputEventMgr();

public:
	void clearEvents();

	/** 添加事件
		@remark 会替换掉同类事件，会替换掉同优先级事件
	*/
	template <typename TEvent>
	TEvent * addEvent()
	{
		TEvent * e = TEvent::create();

		mEventList[e->getPriority()] = TSharedEvent( e );

		return e;
	}
public:
	/** 删除事件
		@remark 根据事件类型 删除事件
	*/
	template <typename TEvent>
	void delEvent()
	{
		for ( TEventList::iterator it = mEventList.begin();
			it != mEventList.end();
			++it )
		{
			TEvent * eve = dynamic_cast<TEvent *>(it->second.get());
			if ( eve )
			{
				mEventList.erase( it );
				break;
			}
		}
	}	
public:
	/** 取得事件
		@remark 根据事件类型 取得事件
	*/
	template <typename TEvent>
	TEvent * getEvent()
	{
		for ( TEventList::iterator it = mEventList.begin();
			it != mEventList.end();
			++it )
		{
			TEvent * eve = dynamic_cast<TEvent *>(it->second.get());
			if ( eve )
			{
				return eve;
			}
		}
		return NULL;
	}
public:
	void LButtonDown(UINT nFlags, CPoint point);

	void LButtonUp(UINT nFlags, CPoint point);

	void RButtonDown(UINT nFlags, CPoint point);

	void RButtonUp(UINT nFlags, CPoint point);

	void MButtonDown(UINT nFlags, CPoint point);

	void MButtonUp(UINT nFlags, CPoint point);

	void MouseMove(UINT nFlags, CPoint delta, CPoint point);

	void MouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void KeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	TEventList	mEventList;
};

#endif


/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-10
* ����: Mfc�����¼�������
******************************************************************************/
#ifndef MfcInputEventMgr_H
#define MfcInputEventMgr_H


#include "MfcInputEvent.h"


/** MFC�����¼�������
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

	/** ����¼�
		@remark ���滻��ͬ���¼������滻��ͬ���ȼ��¼�
	*/
	template <typename TEvent>
	TEvent * addEvent()
	{
		TEvent * e = TEvent::create();

		mEventList[e->getPriority()] = TSharedEvent( e );

		return e;
	}
public:
	/** ɾ���¼�
		@remark �����¼����� ɾ���¼�
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
	/** ȡ���¼�
		@remark �����¼����� ȡ���¼�
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


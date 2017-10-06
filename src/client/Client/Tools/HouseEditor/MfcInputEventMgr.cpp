#include "HouseEditorPCH.h"

#include "MfcInputEventMgr.h"
#include "EQMain.h"

//-----------------------------------------------------------------------
template<> MfcInputEventMgr* Ogre::Singleton<MfcInputEventMgr>::ms_Singleton = 0;

//-----------------------------------------------------------------------
MfcInputEventMgr::MfcInputEventMgr()
{
}
//-----------------------------------------------------------------------
MfcInputEventMgr::~MfcInputEventMgr()
{
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::clearEvents()
{
	mEventList.clear();
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::LButtonDown(UINT nFlags, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->LButtonDown(nFlags, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::LButtonUp(UINT nFlags, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->LButtonUp(nFlags, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::RButtonDown(UINT nFlags, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->RButtonDown(nFlags, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::RButtonUp(UINT nFlags, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->RButtonUp(nFlags, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::MButtonDown(UINT nFlags, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->MButtonDown(nFlags, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::MButtonUp(UINT nFlags, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->MButtonUp(nFlags, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::MouseMove(UINT nFlags, CPoint delta, CPoint point)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->MouseMove(nFlags, delta, point, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::MouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->MouseWheel(nFlags, zDelta, pt, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->KeyDown(nChar, nRepCnt, nFlags, processed);
	}
}
//-----------------------------------------------------------------------
void MfcInputEventMgr::KeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool processed = false;
	TEventList::reverse_iterator it;
	for ( it =  mEventList.rbegin(); it != mEventList.rend(); ++it )
	{
		it->second->KeyUp(nChar, nRepCnt, nFlags, processed);
	}
}
//-----------------------------------------------------------------------





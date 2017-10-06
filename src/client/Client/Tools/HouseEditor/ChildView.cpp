// ChildView.cpp : CChildView 类的实现
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "ChildView.h"
#include "SceneManager.h"
#include "EQLog.h"
#include "MfcInputEventMgr.h"
#include "RootManager.h"


#include "GraphicsSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


CChildView * g_ChildView = NULL;

CChildView::CChildView() 
{
	g_ChildView = this;
}

CChildView::~CChildView()
{
}





// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	GraphicsSystem::getSingleton().render();
}



BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GraphicsSystem::getSingleton().sizeChange();
}



BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	MfcInputEventMgr::getSingleton().MouseWheel(nFlags, zDelta, pt);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	MfcInputEventMgr::getSingleton().LButtonDown(nFlags, point);

	CWnd::OnLButtonDown(nFlags, point);
	SetFocus();
	SetCapture();
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	MfcInputEventMgr::getSingleton().LButtonUp(nFlags, point);

	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	MfcInputEventMgr::getSingleton().RButtonDown(nFlags, point);

	CWnd::OnRButtonDown(nFlags, point);
	SetFocus();
	SetCapture();
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	MfcInputEventMgr::getSingleton().RButtonUp(nFlags, point);

	CWnd::OnRButtonUp(nFlags, point);
	ReleaseCapture();
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	int xt = point.x - mLastMouseMovePos.x;
	int yt = point.y - mLastMouseMovePos.y;

	CPoint delta(xt,yt);

	MfcInputEventMgr::getSingleton().MouseMove(nFlags, delta, point);


	CWnd::OnMouseMove(nFlags, point);

	mLastMouseMovePos = point;

	TRACKMOUSEEVENT       tme;      
	tme.cbSize=sizeof(TRACKMOUSEEVENT);      
	tme.dwFlags=TME_HOVER       |       TME_LEAVE;      
	tme.dwHoverTime=1;    
	tme.hwndTrack=m_hWnd;  
	_TrackMouseEvent(&tme);   
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CChildView::OnMButtonDown(UINT nFlags, CPoint point)
{
	MfcInputEventMgr::getSingleton().MButtonDown(nFlags,point);

	CWnd::OnMButtonDown(nFlags, point);
	SetFocus();
	SetCapture();
}

void CChildView::OnMButtonUp(UINT nFlags, CPoint point)
{
	MfcInputEventMgr::getSingleton().MButtonUp(nFlags, point);

	CWnd::OnMButtonUp(nFlags, point);
	ReleaseCapture();
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	MfcInputEventMgr::getSingleton().KeyDown(nChar, nRepCnt, nFlags);


	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	MfcInputEventMgr::getSingleton().KeyUp(nChar, nRepCnt, nFlags);

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CChildView::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseHover(nFlags, point);

	RootManager::getSingleton().showHitPoint(true);
}

void CChildView::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseLeave();

	RootManager::getSingleton().showHitPoint(false);
}

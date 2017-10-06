// ChildView.cpp : C3DView 类的实现
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"
#include "3DView.h"
#include "Avatar.h"
#include "MainFrm.h"

#include "GraphicsSystem.h"

#include "NodeAdjuster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(C3DView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()




// C3DView

C3DView::C3DView() 
	: mLastMouseMovePos( 0, 0 )
{
}

C3DView::~C3DView()
{
}


// C3DView 消息处理程序

BOOL C3DView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void C3DView::OnPaint() 
{
	CPaintDC dc(this);
	// 不要为绘制消息而调用 CWnd::OnPaint()

	GraphicsSystem::getSingleton().render();		
}



BOOL C3DView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void C3DView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	GraphicsSystem::getSingleton().sizeChange();
}



BOOL C3DView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	float zt = (float)zDelta/120.0f*50;
	Ogre::Camera* cam = GraphicsSystem::getSingleton().getCamera();
	Ogre::Vector3 vec = cam->getPosition();
	vec.z -= zt;
	cam->setPosition(vec);	

	GraphicsSystem::getSingleton().getNodeAdjuster()->autoScaleXYZ();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void C3DView::OnLButtonDown(UINT nFlags, CPoint point)
{	
	CWnd::OnLButtonDown(nFlags, point);
	SetFocus();
	switch ( static_cast<CMainFrame *>(::AfxGetMainWnd() )->getPage() )
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		GraphicsSystem::getSingleton().getNodeAdjuster()->pickObject( point.x, point.y, GraphicsSystem::getSingleton().getCamera() );	
		break;
	case 4:
		GraphicsSystem::getSingleton().getNodeAdjuster()->pickObject( point.x, point.y, GraphicsSystem::getSingleton().getCamera() );	
		break;
	default:
		break;
	}
}

void C3DView::OnLButtonUp(UINT nFlags, CPoint point)
{
	GraphicsSystem::getSingleton().getNodeAdjuster()->resetMaterial();

	CWnd::OnLButtonUp(nFlags, point);
}

void C3DView::OnRButtonDown(UINT nFlags, CPoint point)
{	
	CWnd::OnRButtonDown(nFlags, point);
	SetFocus();	
}

void C3DView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
}

void C3DView::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( mLastMouseMovePos == CPoint(0,0) )
		mLastMouseMovePos = point;

	int xt = point.x - mLastMouseMovePos.x;
	int yt = point.y - mLastMouseMovePos.y;
	mLastMouseMovePos = point;

	Ogre::SceneNode * node = GraphicsSystem::getSingleton().getItemNode();
	switch ( static_cast<CMainFrame *>(::AfxGetMainWnd() )->getPage() )
	{
	case 1:
		if ( g_Avatar )
		{
			node = g_Avatar->getModel()->getMainNode();
		}
	case 2:
		{
			if ( node )
			{
				if ( nFlags & MK_LBUTTON )
				{
					// 处理旋转
					Ogre::Radian rad((Ogre::Real)xt/60.0f);
					node->yaw(rad);

					rad = (Ogre::Real)yt/60.0f;
					node->pitch(rad,Ogre::Node::TS_PARENT);
				}
				else if ( nFlags & MK_RBUTTON )
				{
					// 处理移动
					const float baseDis = 400.0f;
					const float dis = GraphicsSystem::getSingleton().getCamera()->getPosition().distance( node->getPosition() );
					
					xt = xt * dis / baseDis;
					yt = yt * dis / baseDis;
		
					GraphicsSystem::getSingleton().getSceneMgr()->getRootSceneNode()->translate( xt/2.0f, -yt/2.0f, 0 );			
				}
			}
		}
		break;
	case 3:
	case 4:
		{
			if ( nFlags & MK_LBUTTON )
			{
				CRect rc;
				this->GetClientRect( rc );				

				GraphicsSystem::getSingleton().getNodeAdjuster()->onMouseDrag( float(xt)/rc.Width(), float(yt)/rc.Height() );

				if ( g_HouseComp )
				{
					static_cast<CMainFrame *>(::AfxGetMainWnd())->m_wndModelDlg.syncNodeInfo();
				}
			}
			else if ( nFlags & MK_MBUTTON )
			{
				if ( GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000 )
				{
					//旋转场景点
					float factor = 0.005f;

					GraphicsSystem::getSingleton().getGridNode()->rotate( Ogre::Vector3::UNIT_Y, Ogre::Radian( factor * xt ) );
					GraphicsSystem::getSingleton().getSceneMgr()->getRootSceneNode()->rotate( Ogre::Vector3::UNIT_X, Ogre::Radian( factor * yt ) );
				}
				else
				{
					//平移摄像机
					float factor = 400.0f;

					CRect rc;
					this->GetClientRect( &rc );
					
					Ogre::Vector3 vec(-factor*xt/rc.Width(), factor*yt/rc.Height(), 0 );
					Ogre::Camera * cam = GraphicsSystem::getSingleton().getCamera();
					cam->move( cam->getOrientation() * vec );					
				}			
			}

		}
		break;
	default:
		break;
	}


	CWnd::OnMouseMove(nFlags, point);
}



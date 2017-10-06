// MainFrm.cpp : CMainFrame 类的实现
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"

#include "MainFrm.h"
#include "EQMain.h"
#include "HSCManager.h"
#include "HouseScenecomponent.h"
#include "Avatar.h"
#include "MyMsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_AVATAR_VIEW, &CMainFrame::OnShowAvatarView)
	ON_COMMAND(ID_MODEL_VIEW, &CMainFrame::OnShowModelView)
	//ON_COMMAND(ID_EDIT_VIEW, &CMainFrame::OnShowObjectEditView)
	ON_COMMAND(ID_HOUSE_VIEW, &CMainFrame::OnShowHouseView)

	//ON_COMMAND(ID_BUTTON_CREATENEW, &CMainFrame::OnCreateNewScene)
	//ON_COMMAND(ID_BUTTON_OPEN, &CMainFrame::OnOpenScene)
	//ON_COMMAND(ID_BUTTON_SAVE, &CMainFrame::OnSaveScene)
	//ON_COMMAND(ID_BUTTON_SAVEAS, &CMainFrame::OnSaveAsScene)
	//ON_COMMAND(ID_BUTTON_CLOSE, &CMainFrame::OnCloseScene)
	ON_COMMAND(ID_BUTTON_SELECT, &CMainFrame::OnSelectMode)
	ON_COMMAND(ID_BUTTON_MOVE, &CMainFrame::OnMoveMode)
	ON_COMMAND(ID_BUTTON_ROTATE, &CMainFrame::OnRotateMode)
	ON_COMMAND(ID_BUTTON_SCALE, &CMainFrame::OnScaleMode)
	//ON_COMMAND(ID_BUTTON_PREV, &CMainFrame::OnPrevStep)
	//ON_COMMAND(ID_BUTTON_NEXT, &CMainFrame::OnNextStep)
	//ON_COMMAND(ID_BUTTON_LOCK, &CMainFrame::OnLockModel)
	//ON_COMMAND(ID_BUTTON_MOVENEAR, &CMainFrame::OnMoveNear)

	ON_COMMAND(ID_SHOWXML, &CMainFrame::OnShowxml)
	ON_COMMAND(ID_COMMON_KEY, &CMainFrame::OnCommonKey)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
	: mPage( 0 )
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	// 创建3d视图
	if (!m_wnd3DView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	/////////////创建相关控制条///////////////
	m_wndRoleRaceBar.Create("角色面板", this, CSize(200,300),TRUE, IDC_CHARBAR);
	m_wndRoleRaceBar.SetBarStyle(m_wndRoleRaceBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndModelBar.Create("模型面板", this, CSize(200,300),TRUE, 0);
	m_wndModelBar.SetBarStyle(m_wndModelBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndRoleSectionBar.Create("换装面板", this, CSize(200,400),TRUE, IDC_AVATARBAR);
	m_wndRoleSectionBar.SetBarStyle(m_wndRoleSectionBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	m_wndAnimationBar.Create("动作面板", this, CSize(500,100),TRUE, IDC_ANIMATIONBAR);
	m_wndAnimationBar.SetBarStyle(m_wndAnimationBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndPropertyBar.Create("属性面板", this, CSize(250,250),TRUE, 0);
	m_wndPropertyBar.SetBarStyle(m_wndPropertyBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	///////////////创建相关对话框////////////
	m_wndRoleRaceDlg.Create( CRoleRaceDlg::IDD, &m_wndRoleRaceBar );
	m_wndRoleSectionDlg.Create( CRoleSectionDlg::IDD, &m_wndRoleSectionBar );
	m_wndAnimationDlg.Create( CAnimationDlg::IDD, &m_wndAnimationBar );

	
	m_wndModelDlg.Create( CModelDlg::IDD, &m_wndModelBar );
	m_wndPropertyDlg.Create( NULL, "属性窗口", WS_BORDER | WS_VISIBLE, CRect(0, 0, 200, 200), &m_wndPropertyBar, 0 );
	
	// 设置属性消息的处理者
	m_wndPropertyDlg.SetOwner( &m_wndModelDlg );

	//====================================================================
	///////////////停靠控制///////////////////
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndRoleRaceBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndModelBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndRoleSectionBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndAnimationBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPropertyBar.EnableDocking(CBRS_ALIGN_ANY);
	
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndRoleRaceBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndModelBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndRoleSectionBar, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndAnimationBar, AFX_IDW_DOCKBAR_BOTTOM);
	DockControlBar(&m_wndPropertyBar, AFX_IDW_DOCKBAR_RIGHT);

	/////////////调整工具条/////////////////
	initToolBarItems();


	// 设置默认的视图
	return 0;
}

void CMainFrame::initToolBarItems()
{
	m_wndToolBar.SetSizes(CSize(48,48), CSize(24,24));

	int i = -1;

	++i;
	m_wndToolBar.SetButtonText( i,"新建" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_CREATENEW, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonText( i,"打开" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_OPEN, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonText( i,"保存" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_SAVE, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonText( i,"另存为" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_SAVEAS, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonText( i,"关闭" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_CLOSE, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonInfo( i, 0, TBBS_SEPARATOR, i );

	++i;
	m_wndToolBar.SetButtonText( i,"选择" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_SELECT, TBBS_CHECKGROUP, i );

	++i;
	m_wndToolBar.SetButtonText( i,"移动" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_MOVE, TBBS_CHECKGROUP, i );

	++i;
	m_wndToolBar.SetButtonText( i,"旋转" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_ROTATE, TBBS_CHECKGROUP, i );

	++i;
	m_wndToolBar.SetButtonText( i,"缩放" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_SCALE, TBBS_CHECKGROUP, i );

	++i;
	m_wndToolBar.SetButtonInfo( i, 0, TBBS_SEPARATOR, i );

	++i;
	m_wndToolBar.SetButtonText( i,"上一步" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_PREV, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonText( i,"下一步" );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_NEXT, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonInfo( i, 0, TBBS_SEPARATOR, i );

	//++i;
	//m_wndToolBar.SetButtonText( i," 锁定 " );
	//m_wndToolBar.SetButtonInfo( i, ID_BUTTON_LOCK, TBBS_BUTTON, i );

	//++i;
	//m_wndToolBar.SetButtonText( i," 贴近 " );
	//m_wndToolBar.SetButtonInfo( i, ID_BUTTON_MOVENEAR, TBBS_BUTTON, i );

}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG




BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//// 让视图第一次尝试该命令
	if (m_wnd3DView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::doBeforeAvatarCreate()
{
	this->m_wndAnimationDlg.resetContent();
	this->m_wndRoleSectionDlg.resetContent();
}

void CMainFrame::doAfterAvatarCreate()
{
	if ( !g_Avatar )
		return;

	g_Avatar->getModel()->getMainNode()->yaw( Ogre::Degree( -45 ) );

	m_wndRoleSectionDlg.notifyAvatarCreate();
	m_wndAnimationDlg.notifyAvatarCreate();

	

	m_wnd3DView.SetFocus();
}

/** 显示 换装相关面板
*/
void CMainFrame::OnShowAvatarView()
{
	setPage( 1 );	
}

/** 显示 模型相关面板
*/
void CMainFrame::OnShowModelView()
{
	setPage( 2 );	
}

/** 显示 物件编辑相关面板
*/
void CMainFrame::OnShowObjectEditView()
{
	//setPage( 3 );	
}

/** 显示 房屋部件相关面板
*/
void CMainFrame::OnShowHouseView()
{
	setPage( 4 );	
}

void CMainFrame::clearPage()
{
	// 换装相关面板
	this->ShowControlBar( &m_wndRoleRaceBar, FALSE, FALSE );
	this->ShowControlBar( &m_wndRoleSectionBar, FALSE, FALSE );
	this->ShowControlBar( &m_wndAnimationBar, FALSE, FALSE );

	// 隐藏物品窗口
	this->m_wndRoleSectionDlg.m_wndItemDlg.ShowWindow( SW_HIDE );

	if ( GraphicsSystem::getSingleton().hasSetup() )
	{
		GraphicsSystem::getSingleton().getGridNode()->setPosition( 0, 0, 0 );
		GraphicsSystem::getSingleton().getGridNode()->setOrientation( 1, 0, 0, 0 );

		GraphicsSystem::getSingleton().getSceneMgr()->getRootSceneNode()->setPosition( 0, 0, 0 );
		GraphicsSystem::getSingleton().getSceneMgr()->getRootSceneNode()->setOrientation( 1, 0, 0, 0 );
		
		GraphicsSystem::getSingleton().getItemNode()->setOrientation( 1, 0, 0, 0 );
		GraphicsSystem::getSingleton().getItemNode()->setScale( 1, 1, 1 );
		
		
		GraphicsSystem::getSingleton().getCamera()->setPosition( 0, 0, 400 );
		GraphicsSystem::getSingleton().getCamera()->lookAt( 0, 0,-10000 );	
	}

	// 清除角色
	if ( g_Avatar )
	{
		g_Avatar->getModel()->setVisible( false );
	}

	// 模型显示相关面板
	this->ShowControlBar( &m_wndPropertyBar, FALSE, FALSE );
	this->ShowControlBar( &m_wndModelBar, FALSE, FALSE );
	
	this->ShowControlBar( &m_wndToolBar, FALSE, FALSE );

	// 清空属性
	m_wndModelDlg.resetContent();

	// 清空正在 浏览的物品
	GraphicsSystem::getSingleton().displayItem( 0 );	

	// 还原位置
	Ogre::SceneNode * pNode = GraphicsSystem::getSingleton().getItemNode();
	if ( pNode )
	{
		pNode->setOrientation( 1.0f, 0.0f, 0.0f, 0.0f );
		//pNode->yaw( Ogre::Degree( -45 ) );
	}

	if ( GraphicsSystem::getSingleton().getGrid() )
		GraphicsSystem::getSingleton().getGrid()->setVisible( false );

	// 将选定框藏起来
	GraphicsSystem::getSingleton().getNodeAdjuster()->detach();	


	if ( g_HouseComp )
	{
		HSCManager::getSingleton().destroyHouseSceneComponent( g_HouseComp );
		g_HouseComp = NULL;	
	}

	CMenu * m = this->GetMenu();
	if ( m )
	{
		m->CheckMenuItem( ID_AVATAR_VIEW, MF_UNCHECKED ); 
		m->CheckMenuItem( ID_MODEL_VIEW, MF_UNCHECKED ); 
		m->CheckMenuItem( ID_EDIT_VIEW, MF_UNCHECKED ); 		
		m->CheckMenuItem( ID_HOUSE_VIEW, MF_UNCHECKED ); 		
	}	
}

void CMainFrame::setPage( int val )
{
	if ( getPage() == val )
		return;

	if ( !GraphicsSystem::getSingleton().hasSetup() )
		return;

	clearPage();

	mPage = val;

	switch ( mPage )
	{
	case 1:		// 换装
		{
			this->SetWindowTextA("换装视图");
			// 换装相关面板
			this->ShowControlBar( &m_wndRoleRaceBar, TRUE, FALSE );
			this->ShowControlBar( &m_wndRoleSectionBar, TRUE, FALSE );
			this->ShowControlBar( &m_wndAnimationBar, TRUE, FALSE );

			// 物品位置
			GraphicsSystem::getSingleton().setItemNodePosition( Ogre::Vector3( -100, -80, 0 ) );
			GraphicsSystem::getSingleton().getItemNode()->yaw( Ogre::Degree( -45 ) );


			CMenu * m = this->GetMenu();
			if ( m )
			{
				m->CheckMenuItem( ID_AVATAR_VIEW, MF_CHECKED ); 
			}

			if ( g_Avatar )
			{
				g_Avatar->getModel()->setVisible( true );				
			}
		}
		break;
	case 2:		// 模型显示
		{
			this->SetWindowTextA("模型视图");
			this->ShowControlBar( &m_wndPropertyBar, TRUE, FALSE );
			this->ShowControlBar( &m_wndModelBar, TRUE, FALSE );

			GraphicsSystem::getSingleton().setItemNodePosition( Ogre::Vector3( 0, -80, 0 ) );
			GraphicsSystem::getSingleton().getItemNode()->yaw( Ogre::Degree( -45 ) );

			CMenu * m = this->GetMenu();
			if ( m )
			{
				m->CheckMenuItem( ID_MODEL_VIEW, MF_CHECKED ); 
			}

			this->m_wndModelDlg.loadPage( mPage );
		}
		break;
	case 3:		// 物件编辑
		{
			this->SetWindowTextA("物件编辑视图");
			GraphicsSystem::getSingleton().getGridNode()->yaw( Ogre::Degree( -45 ) );
			GraphicsSystem::getSingleton().getSceneMgr()->getRootSceneNode()->pitch( Ogre::Degree( 25 ) );


			this->ShowControlBar( &m_wndModelBar, TRUE, FALSE );
			this->ShowControlBar( &m_wndToolBar, TRUE, FALSE );

			if ( GraphicsSystem::getSingleton().getGrid() )
				GraphicsSystem::getSingleton().getGrid()->setVisible( true );
			GraphicsSystem::getSingleton().setItemNodePosition( Ogre::Vector3( 0, 0, 0 ) );

			CMenu * m = this->GetMenu();
			if ( m )
			{
				m->CheckMenuItem( ID_EDIT_VIEW, MF_CHECKED ); 
			}
			this->m_wndModelDlg.loadPage( mPage );
		}
		break;
	case 4:		// 房屋部件
		{
			this->SetWindowTextA("房屋部件视图");
			GraphicsSystem::getSingleton().getCamera()->setPosition( 0, 0, 600 );
			GraphicsSystem::getSingleton().getGridNode()->yaw( Ogre::Degree( -45 ) );
			GraphicsSystem::getSingleton().getSceneMgr()->getRootSceneNode()->pitch( Ogre::Degree( 25 ) );

			this->ShowControlBar( &m_wndPropertyBar, TRUE, FALSE );
			this->ShowControlBar( &m_wndModelBar, TRUE, FALSE );
			this->ShowControlBar( &m_wndToolBar, TRUE, FALSE );

			if ( GraphicsSystem::getSingleton().getGrid() )
				GraphicsSystem::getSingleton().getGrid()->setVisible( true );


			GraphicsSystem::getSingleton().setItemNodePosition( Ogre::Vector3( 40, 0, 40 ) );
	
			CMenu * m = this->GetMenu();
			if ( m )
			{
				m->CheckMenuItem( ID_HOUSE_VIEW, MF_CHECKED ); 
			}
			this->m_wndModelDlg.loadPage( mPage );
		}
		break;
	default:
		break;
	}
}

/** 新建场景
*/
afx_msg void CMainFrame::OnCreateNewScene()
{
}

/** 打开场景
*/
afx_msg void CMainFrame::OnOpenScene()
{
}

/** 保存场景
*/
afx_msg void CMainFrame::OnSaveScene()
{
}

/** 另存场景
*/
afx_msg void CMainFrame::OnSaveAsScene()
{
}


/** 关闭场景
*/
afx_msg void CMainFrame::OnCloseScene()
{

}

/** 进入选择模式
*/
afx_msg void CMainFrame::OnSelectMode()
{
	GraphicsSystem::getSingleton().getNodeAdjuster()->setMode( OP_SELECT_NODE );
}


/** 进入移动模式
*/
afx_msg void CMainFrame::OnMoveMode()
{
	GraphicsSystem::getSingleton().getNodeAdjuster()->setMode( OP_MOVE_NODE );
}


/** 进入旋转模式
*/
afx_msg void CMainFrame::OnRotateMode()
{
	GraphicsSystem::getSingleton().getNodeAdjuster()->setMode( OP_ROTATE_NODE );
}


/** 进入缩放模式
*/
afx_msg void CMainFrame::OnScaleMode()
{
	GraphicsSystem::getSingleton().getNodeAdjuster()->setMode( OP_SCALE_NODE );
}


/** 上一步
*/
afx_msg void CMainFrame::OnPrevStep()
{
	
}


/** 下一步
*/
afx_msg void CMainFrame::OnNextStep()
{
	
}


/** 锁定
*/
afx_msg void CMainFrame::OnLockModel()
{
}


/** 贴近
*/
afx_msg void CMainFrame::OnMoveNear()
{
}



void CMainFrame::OnShowxml()
{
	this->m_wndModelDlg.showXml();
}

void CMainFrame::OnCommonKey()
{
	std::string msg;
	msg += " 常用操作：\n";

	msg += "     按住Mouse左键 =====> 旋转人物 \n";
	msg += "     按住Mouse右键 =====> 平移人物 \n";
	msg += "     滚动Mouse中键 =====> 调整相机距离 \n";
	msg += "     按住Mouse中键 =====> 平移画面 \n";
	msg += "     按住Mouse中键 + Alt =====> 绕中心点旋转画面 \n";

	CMyMsgBox::Show( msg );
}

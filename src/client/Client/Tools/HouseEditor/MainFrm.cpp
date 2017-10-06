// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"

#include "MainFrm.h"
#include "EQMain.h"
#include "OgreMaxSceneLoader.h"
#include "SceneManager.h"
#include "HouseResManager.h"
#include "OpenDlg.h"
#include "SaveDlg.h"
#include "MfcInputEventMgr.h"
#include "UIFitmentEvent.h"
#include "HouseScene.h"
#include "HouseSceneSerializer.h"
#include "CreateTerrainDlg.h"
#include "ReshapeTerrainDlg.h"
#include "TerrainData.h"
#include "RootManager.h"
#include "SceneListener.h"
#include "SceneInfo.h"
#include "FarmScript.h"
#include "Terrain.h"
#include "OperatorManager.h"
#include "TerrainGrid.h"
#include "TerrShadowmap.h"
#include "PathSearchDlg.h"
#include "UIOutsideCutEvent.h"


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

CMainFrame * g_MainFrame = NULL;
bool g_MainFrameClosing = false;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_IMPORTSCENE, &CMainFrame::OnAppImportscene)
	ON_COMMAND(ID_APP_TERRAIN, &CMainFrame::OnAppTerrain)
	ON_COMMAND(ID_APP_SAVEHOUSE, &CMainFrame::OnAppSavehouse)
	ON_COMMAND(ID_APP_CLOSE, &CMainFrame::OnAppClose)
	ON_COMMAND(ID_APP_OPEN, &CMainFrame::OnAppOpen)
	ON_COMMAND(ID_BUTTON_GRID, &CMainFrame::OnBrowseGridMode)
	ON_COMMAND(ID_BUTTON_COMPONENT, &CMainFrame::OnBrowseComponentMode)
	ON_COMMAND(ID_BUTTON_PROPERTY, &CMainFrame::OnShowProperty)
	ON_COMMAND(ID_SHOW_PANEL, &CMainFrame::OnShowPanel)
	ON_COMMAND(ID_BUTTON_AMBIENT, &CMainFrame::OnSetAmbient)
	ON_COMMAND(ID_BUTTON_LIGHT, &CMainFrame::OnSetLight)
	ON_COMMAND(ID_BUTTON_CREATE_TERRAIN, &CMainFrame::OnCreateTerrain)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TEST_AUTO_FIND_PATH, &CMainFrame::OnTestAutoFindPath)
	ON_COMMAND(ID_LOAD_REMOVE_FARM_SCENE, &CMainFrame::OnLoadRemoveFarmScene)
	ON_COMMAND(ID_RUN_FARM_SCRIPT, &CMainFrame::OnRunFarmScript)
	ON_UPDATE_COMMAND_UI(ID_RUN_FARM_SCRIPT, &CMainFrame::OnUpdateRunFarmScript)
	ON_UPDATE_COMMAND_UI(ID_LOAD_REMOVE_FARM_SCENE, &CMainFrame::OnUpdateLoadRemoveFarmScene)
	ON_COMMAND(ID_EDIT_UNDO, &CMainFrame::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMainFrame::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMainFrame::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMainFrame::OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_TERRAIN_SHADOWMAP, &CMainFrame::OnUpdateTerrainShadowmap)
	ON_COMMAND(ID_TERRAIN_SHADOWMAP, &CMainFrame::OnTerrainShadowmap)
	//ON_COMMAND(ID_BUTTON_POSTPROCESS, &CMainFrame::OnSetPostProcess)
	ON_COMMAND(ID_BUTTON_TERRAINSHOT, &CMainFrame::OnTerrainShot)
	ON_COMMAND(ID_PROJ_VIEW, &CMainFrame::OnProjView)
	ON_UPDATE_COMMAND_UI(ID_TEST_OUTSIDE_CUT, &CMainFrame::OnUpdateTestOutsideCut)
	ON_COMMAND(ID_TEST_OUTSIDE_CUT, &CMainFrame::OnTestOutsideCut)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_RAY_POS,
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};





CMainFrame::CMainFrame() 
: mFromOgreMaxScene(false)
, mLastResFileName( "temp/temp.scene" )
, mOpening(false)
{
	g_MainFrame = this;
	GraphicsSystem::getSingleton().addListener(this);
	this-> m_bAutoMenuEnable   =   false; 
}

CMainFrame::~CMainFrame()
{
	GraphicsSystem::getSingleton().removeListener(this);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	//m_wndStatusBar.SetPaneInfo(0,ID_SEPARATOR,SBPS_NORMAL,??)

	/////////////��������///////////////
	m_wndLeftBar.Create("�����", this, CSize(200,300),TRUE, IDC_LEFTBAR);
	m_wndLeftBar.SetBarStyle(m_wndLeftBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndRightBar.Create("�����", this, CSize(200,400),TRUE, IDC_RIGHTBAR);
	m_wndRightBar.SetBarStyle(m_wndRightBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndBottomBar.Create("�����", this, CSize(500,100),TRUE, IDC_BOTTOMBAR);
	m_wndBottomBar.SetBarStyle(m_wndBottomBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndPropertyBar.Create("�������", this, CSize(280,270),TRUE, 0);
	m_wndPropertyBar.SetBarStyle(m_wndPropertyBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndAmbientBar.Create("���������", this, CSize(250,335),TRUE, 0);
	m_wndAmbientBar.SetBarStyle(m_wndAmbientBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndLightBar.Create("�ƹ����", this, CSize(280,500),TRUE, 0);
	m_wndLightBar.SetBarStyle(m_wndLightBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

//	m_wndPostProcessBar.Create("��Ч���", this, CSize(250,270),TRUE, 0);
//	m_wndPostProcessBar.SetBarStyle(m_wndPostProcessBar.GetBarStyle() |
//		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	/////////////�����������//////////////
	//�������
	m_wndPropertyDlg.Create( NULL, "���Դ���", WS_BORDER | WS_VISIBLE, CRect(0, 0, 200, 200), &m_wndPropertyBar, 0 );
	//���������
	m_wndAmbientDlg.Create( NULL, "�����ⴰ��", WS_BORDER | WS_VISIBLE, CRect(0, 0, 200, 200), &m_wndAmbientBar, 0 );
	//�ƹ����
	m_wndLightDlg.Create( NULL, "�ƹⴰ��", WS_BORDER | WS_VISIBLE, CRect(0, 0, 200, 200), &m_wndLightBar, 0 );
	m_wndLightAdjust.Create(CLightAdjustDlg::IDD, &m_wndLightBar);
	//��Ч���
//	m_wndPostProcessDlg.Create( NULL, "��Ч����", WS_BORDER | WS_VISIBLE, CRect(0, 0, 200, 200), &m_wndPostProcessBar, 0 );

	//�����
	m_wndResourceDlg.Create( CResourceDlg::IDD, &m_wndLeftBar );	

	//�����
	m_wndRightTabCtrl.Create(TCS_UP|WS_CHILD|WS_VISIBLE,CRect(0,0,200,400),&m_wndRightBar,IDC_RIGHTTABCTRL);

	m_wndHouseCompDlg.Create(CHouseCompDlg::IDD, &m_wndRightTabCtrl);
	m_wndHouseGridDlg.Create(CHouseGridDlg::IDD, &m_wndRightTabCtrl);
	m_wndBrushSelectorDlg.Create(CBrushSelectorDlg::IDD, &m_wndRightTabCtrl);
	m_wndSceneObjectDlg.Create(CSceneObjectDlg::IDD, &m_wndRightTabCtrl);
	m_wndCPathSearchDlg.Create(CPathSearchDlg::IDD, &m_wndRightTabCtrl);

	m_wndRightTabCtrl.AddPage(&m_wndHouseGridDlg,"����");
	m_wndRightTabCtrl.AddPage(&m_wndHouseCompDlg,"��װ");
	m_wndRightTabCtrl.AddPage(&m_wndBrushSelectorDlg,"����");
	m_wndRightTabCtrl.AddPage(&m_wndSceneObjectDlg,"���");
	m_wndRightTabCtrl.AddPage(&m_wndCPathSearchDlg,"Ѱ·");
	m_wndRightTabCtrl.UpdateWindow();

	//�����
	m_wndBottomCommDlg.Create(CBottomCommDlg::IDD, &m_wndBottomBar);


	//====================================================================
	///////////////ͣ������///////////////////
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndLeftBar.EnableDocking(CBRS_ALIGN_LEFT);
	m_wndPropertyBar.EnableDocking(CBRS_ALIGN_LEFT);
	m_wndAmbientBar.EnableDocking(CBRS_ALIGN_RIGHT);
	m_wndLightBar.EnableDocking(CBRS_ALIGN_RIGHT);
	m_wndRightBar.EnableDocking(CBRS_ALIGN_RIGHT);
	m_wndBottomBar.EnableDocking(CBRS_ALIGN_BOTTOM);
//	m_wndPostProcessBar.EnableDocking(CBRS_ALIGN_RIGHT);


	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndLeftBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndRightBar, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndBottomBar, AFX_IDW_DOCKBAR_BOTTOM);

	m_wndPropertyBar.ShowWindow( SW_HIDE );
	FloatControlBar(&m_wndPropertyBar, CPoint(900, 600));
	m_wndAmbientBar.ShowWindow( SW_HIDE );
	FloatControlBar(&m_wndAmbientBar, CPoint(400, 300));
	m_wndLightBar.ShowWindow( SW_HIDE );
	FloatControlBar(&m_wndLightBar, CPoint(200, 100));
// 	m_wndPostProcessBar.ShowWindow( SW_HIDE );
// 	FloatControlBar(&m_wndPostProcessBar, CPoint(400, 300));

	// ����������Ϣ�Ĵ�����
	m_wndPropertyDlg.SetOwner( &m_wndResourceDlg );
	m_wndAmbientDlg.SetOwner( &m_wndResourceDlg );
	m_wndLightDlg.SetOwner( &m_wndResourceDlg );
	m_wndLightAdjust.SetOwner( &m_wndResourceDlg );
//	m_wndPostProcessDlg.SetOwner( &m_wndResourceDlg );

	initToolBarItems();

	// ���س���������
	showPanels( false );
	return 0;
}

void CMainFrame::notifySystemSetup()
{
	m_wndHouseGridDlg.notifySystemSetup();
	m_wndHouseCompDlg.notifySystemSetup();
	m_wndSceneObjectDlg.notifySystemSetup();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame ���

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


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnAppImportscene()
{
	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	if ( scene && scene->isLoadHouse() )
	{
		MessageBox("������Դ�Ѿ����ع���!", "���ݼ���ʧ��!");
		return;
	}

	//�ļ��Ի��򴴽�
	CFileDialog   Filedlg(TRUE,         //���ļ�
		NULL,        
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,         //OFN_NOCHANGEDIR��ʶ���ı乤��·�� 
		"OgreMax Scene(*.scene)|*.scene||",        
		NULL);    

	if( Filedlg.DoModal() == IDOK )        
	{
		std::string fullName = Filedlg.GetPathName();  
		std::string baseName;
		std::string pathName;
		Ogre::StringUtil::splitFilename(fullName, baseName, pathName);

		//
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
			pathName, "FileSystem", "OgreMax");
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("OgreMax");

		//
		HouseResPtr ptr = OgreMaxSceneLoader::directLoadToHouseType(baseName);

		if ( ptr.isNull() )
		{
			MessageBox("���ļ�ʧ��!");
			return;
		}

		mFromOgreMaxScene = true;

		Ogre::Camera* cam = GraphicsSystem::getSingleton().getCamera();
		////HouseScene* scene = EQSceneManager::getSingleton().createHouseScene(ptr->getName(), cam, true);
		//HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
		//bool bsce = false;
		//if ( !scene )
		//{
		//	scene = EQSceneManager::getSingleton().createNullHouseScene(cam);
		//	bsce = true;
		//}
		HouseSceneSerializer ser;
		ser.createHouseByHouseResName(ptr->getName(), cam);


		OnSceneCreate( true, false);
	}
}



void newTerrainScene(int width, int depth)
{
	const int tileSize = 32;		//32��Ϊһ���ؿ�
	const float scale = 80.0;		//80����Ϊһ������
	const std::string baseTexture = "BaseTerrSurfaceMud.jpg"; //������ĵ������

	//
	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();
	TerrainData* data = EQSceneManager::getSingleton().getTerrainData();
	if ( data->mTextures.size() != 0 )
		return;

	data->mXSize = width;
	data->mZSize = depth;
	data->mTileSize = tileSize;
	data->mScale = Ogre::Vector3(scale,scale,scale);
	data->mPosition = Ogre::Vector3(- data->mScale.x * data->mXSize / 2,0,- data->mScale.z * data->mZSize / 2);
	data->_updateDerivedFields();

	data->mMaterialTemplates.clear();
	data->mMaterialTemplates["OneLayer"] = "Terrain/OneLayer";
	data->mMaterialTemplates["OneLayerLightmap"] = "Terrain/OneLayerLightmap";
	data->mMaterialTemplates["TwoLayer"] = "Terrain/TwoLayer";
	data->mMaterialTemplates["TwoLayerLightmap"] = "Terrain/TwoLayerLightmap";

	data->mPixmaps.clear();
	data->mTextures.clear();
	if (!baseTexture.empty())
	{
		data->mPixmaps.push_back(TerrainData::Pixmap(0));
		data->mTextures.push_back(baseTexture);
	}


	TerrainData::GridInfo gridInfo = { 0, 0, 0, 0, 0 };
	if (!data->mPixmaps.empty())
		gridInfo.layers[0].pixmapId = 1;
	std::fill(data->mGridInfos.begin(), data->mGridInfos.end(), gridInfo);

	//��ʼ��, �ҽ���ͼ��
	scene->initialise(true);
}

void CMainFrame::OnAppTerrain()
{

	SceneInfo* scene = EQSceneManager::getSingleton().getSceneInfo();

	if ( scene->isTerrainEmpty() ) //����Ϊ��, ���߳�����δ��������. �򴴽�����
	{
		CCreateTerrainDlg dlg;
		if( dlg.DoModal() != IDOK )
			return;


		int width = dlg.mWidth;
		int depth = dlg.mDepth;
		if ( width < 1 || width > 200 || depth < 1 || depth > 200 )
		{
			assert(0);
			return;
		}


		newTerrainScene(width, depth);

		OnSceneCreate(false, true);

		return;
	}
	else	//���ݵ���
	{
		CReshapeTerrainDlg dlg;
		if( dlg.DoModal() != IDOK )
			return;

		int left = dlg.mLeft;
		int right = dlg.mRight;
		int top = dlg.mTop;
		int down = dlg.mDown;

		int rLeft = dlg.mRLeft;
		int rRight = dlg.mRRight;
		int rTop = dlg.mRTop;
		int rDown = dlg.mRDown;

		const std::string baseTexture = "BaseTerrSurfaceMud.jpg"; //������ĵ������

		RootManager::getSingleton().getOperatorManager()->reset();

		TerrainData* terrainData = EQSceneManager::getSingleton().getTerrainData();
		int pixmap = terrainData->_registerPixmap(baseTexture,0,0,1,1);

		terrainData->_reshape(rLeft,rTop,rRight,rDown,pixmap);
		EQSceneManager::getSingleton().getTerrain()->buildGeometry(EQSceneManager::getSingleton().getSceneInfo()->getSceneNode(), true);

		SceneListenerManager::getSingleton()._fireSceneReset();

		return;
	}
}

void CMainFrame::OnAppClose()
{
	if ( !mOpening )
		return;

	OnSceneDestroy();

	GraphicsSystem::getSingleton().getSceneMgr()->setSkyBox(false, "");
	GraphicsSystem::getSingleton().getSceneMgr()->setFog(Ogre::FOG_NONE);
	Ogre::ManualObject* edge = RootManager::getSingleton().getSceneEdgeLine();
	if(edge)
		edge->setVisible(false);

	//ɾ������
	EQSceneManager::getSingleton().destroyScene();
}

void CMainFrame::OnAppSavehouse()
{
	//��Ҫ�Ľ�
	if ( !mOpening )
		return;

	HouseSceneSerializer serial;

	std::string str;
	if ( !serial.canSave(str) )
	{
		MessageBox(str.c_str(), "����");
		return;
	}

	std::string fileName = CSaveDlg::Execute( "������Դ", mLastResFileName );

	if ( fileName.empty() )
	{
		return;
	}

	//���Ż��µ���
	TerrainData* data = EQSceneManager::getSingleton().getTerrainData();
	if ( !data->isEmpty() )
	{
		if (data->_optimize())
		{
			EQSceneManager::getSingleton().getTerrain()->buildGeometry( EQSceneManager::getSingleton().getSceneInfo()->getSceneNode(), true);
		}
	}

	//�����ļ���
	if ( !Ogre::StringUtil::endsWith( fileName, ".scene" ) )
		fileName += ".scene";

	mLastResFileName = fileName;

	//�����߼���
	std::string logicName = fileName;
	logicName.erase(logicName.length() - (sizeof(".scene") - 1));

	// ·��
	static const std::string s_sceneDir = "../../Product/Media/Scene/";

	// ���ֳ���
	serial.save(fileName, s_sceneDir);

	// ����������Ϣ
	saveRegion(logicName, s_sceneDir);

	// ���浼������
	saveNav(logicName, s_sceneDir);

}

void CMainFrame::saveRegion(const std::string& name, const std::string& path)
{
	std::string filename = path + name + ".region";

	RootManager::getSingleton().getRegionEditManager()->SaveRegionToFile(filename);
}

void CMainFrame::loadRegion(const std::string& name)
{
	std::string filename = name + ".region";
	m_wndCPathSearchDlg.loadRegion(filename);
}

void CMainFrame::saveNav(const std::string& name, const std::string& path)
{
	std::string filename = path + name + ".nav";
	m_wndCPathSearchDlg.saveNav(filename);
}

void CMainFrame::loadNav(const std::string& name)
{
	std::string filename = name + ".nav";
	m_wndCPathSearchDlg.loadNav(filename);

	if ( !m_wndCPathSearchDlg.IsWindowVisible() )
		m_wndCPathSearchDlg.getNavMeshLineNode()->setVisible(false);

}

void CMainFrame::OnAppOpen()
{
	if ( mOpening )
	{
		MessageBox("���ȹرճ���", "��ʾ");
		return;
	}

	COpenDlg::Execute();
}

void CMainFrame::OpenScene(std::string fileName)
{
	if ( fileName.empty() )
	{
		return;
	}

	//�����Դ
	if ( !ExistResource(fileName) )
	{
		MessageBox("��Դ������!", "��ʾ");
		return;
	}
	mLastResFileName = fileName;

	//�����߼���
	std::string logicName = fileName;
	if (Ogre::StringUtil::endsWith(logicName, ".scene"))
		logicName.erase(logicName.length() - (sizeof(".scene") - 1));


	//��������
	Ogre::Camera* cam = GraphicsSystem::getSingleton().getCamera();
	EQSceneManager::getSingleton().loadHouseAndScene(fileName, cam, true, false);

	bool bhouse = false;
	bool bterrain = false;
	if ( EQSceneManager::getSingleton().getHouseScene() )
		bhouse = true;
	if ( !EQSceneManager::getSingleton().getSceneInfo()->isTerrainEmpty() )
		bterrain = true;

	//��ȡ������Ϣ
	loadRegion(logicName);

	//��ȡ��������
	loadNav(logicName);

	OnSceneCreate(bhouse, bterrain);

	EQSceneManager::getSingleton().turnAllLight(!AmbientMgr::getSingleton().getDayNight());
	GraphicsSystem::getSingleton().setContact(AmbientMgr::getSingleton().mContrast);

	Ogre::ManualObject* edge = RootManager::getSingleton().getSceneEdgeLine();
	if(edge)
		edge->setVisible(true);
}

void CMainFrame::showPanels( bool bShow )
{	
	this->ShowControlBar( &m_wndLeftBar, bShow, FALSE  );
	this->ShowControlBar( &m_wndRightBar, bShow, FALSE  );
	this->ShowControlBar( &m_wndBottomBar, bShow, FALSE  );
}

void CMainFrame::showError( const std::string & errMsg )
{
	m_wndStatusBar.SetPaneText( 0, errMsg.c_str() ); 
}

void CMainFrame::showPos( const std::string& msg )
{
	m_wndStatusBar.SetPaneText( 1, msg.c_str() ); 
}

void CMainFrame::initToolBarItems()
{
	m_wndToolBar.SetSizes(CSize(72,35), CSize(16,15));

	int i = -1;

	++i;
	m_wndToolBar.SetButtonText( i," �� " );
	m_wndToolBar.SetButtonInfo( i, ID_APP_OPEN, TBBS_BUTTON, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," ���뷿�� " );
	m_wndToolBar.SetButtonInfo( i, ID_APP_IMPORTSCENE, TBBS_BUTTON, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," ���� " );
	m_wndToolBar.SetButtonInfo( i, ID_APP_TERRAIN, TBBS_BUTTON, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," ���� " );
	m_wndToolBar.SetButtonInfo( i, ID_APP_SAVEHOUSE, TBBS_BUTTON, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," �ر� " );
	m_wndToolBar.SetButtonInfo( i, ID_APP_CLOSE, TBBS_BUTTON, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," С��ͼ " );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_TERRAINSHOT, TBBS_BUTTON, 0 );

	//++i;
	//m_wndToolBar.SetButtonText( i," ���� " );
	//m_wndToolBar.SetButtonInfo( i, ID_APP_ABOUT, TBBS_BUTTON, i );

	++i;
	m_wndToolBar.SetButtonInfo( i, 0, TBBS_SEPARATOR, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," ���� " );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_PROPERTY, TBBS_BUTTON, 0 );


	++i;
	m_wndToolBar.SetButtonText( i," ���� " );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_GRID, TBBS_CHECKBOX, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," ��� " );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_COMPONENT, TBBS_CHECKBOX, 0 );
	//m_wndToolBar.SetButtonInfo( i, ID_BUTTON_COMPONENT, TBBS_CHECKGROUP, i );

	++i;
	m_wndToolBar.SetButtonText( i," ������ " );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_AMBIENT, TBBS_BUTTON, 0 );

	++i;
	m_wndToolBar.SetButtonText( i," �ƹ� " );
	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_LIGHT, TBBS_CHECKBOX, 0 );

// 	++i;
// 	m_wndToolBar.SetButtonText( i," ��Ч " );
// 	m_wndToolBar.SetButtonInfo( i, ID_BUTTON_POSTPROCESS, TBBS_BUTTON, 0 );

	//++i;
	//m_wndToolBar.SetButtonText( i," �������� " );
	//m_wndToolBar.SetButtonInfo( i, ID_BUTTON_CREATE_TERRAIN, TBBS_BUTTON, 0 );
}

void CMainFrame::OnBrowseGridMode()
{
	if ( !EQSceneManager::getSingleton().getHouseScene() )
		return;

	int idx = m_wndToolBar.CommandToIndex( ID_BUTTON_GRID );
	UINT val = m_wndToolBar.GetButtonStyle( idx );		
	if ( val & TBBS_CHECKED )
	{
		//�����¼�
		MfcInputEventMgr::getSingleton().addEvent<BrowseGridEvent>();

		//����������ʾ
		this->ShowControlBar( &m_wndPropertyBar, TRUE, FALSE  );			
		this->m_wndResourceDlg.showSelectGrid();
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<BrowseGridEvent>();
	}

}

void CMainFrame::OnBrowseComponentMode()
{		
	if ( !EQSceneManager::getSingleton().getHouseScene() )
		return;

	int idx = m_wndToolBar.CommandToIndex( ID_BUTTON_COMPONENT );
	UINT val = m_wndToolBar.GetButtonStyle( idx );		
	if ( val & TBBS_CHECKED )
	{
		//�����¼�
		MfcInputEventMgr::getSingleton().addEvent<BrowseComponentEvent>();

		//���������ʾ
		this->ShowControlBar( &m_wndPropertyBar, TRUE, FALSE  );			
		this->m_wndResourceDlg.showSelectComponent();
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<BrowseComponentEvent>();
	}
}

void CMainFrame::OnShowProperty()
{
	this->ShowControlBar( &m_wndPropertyBar, !m_wndPropertyBar.IsVisible(), FALSE  );
}

void CMainFrame::UnCheckedToolBarItem( int barID )
{	
	int idx = m_wndToolBar.CommandToIndex( barID );
	UINT st = m_wndToolBar.GetButtonStyle( idx );	

	st = st & (~TBBS_CHECKED);
	m_wndToolBar.SetButtonStyle( idx, st );
}

void CMainFrame::OnClose()
{
	g_MainFrameClosing = true;

	// �ر�ǰ ��������������ص��¼�
	::MfcInputEventMgr::getSingleton().clearEvents();

	CFrameWnd::OnClose();
}

void CMainFrame::OnSceneCreate(bool house, bool terrain)
{
	//������δ���ܴ�
	RootManager::getSingleton().getOperatorManager()->reset();

	// �������α߽�
	RootManager::getSingleton().reshapeTerrainEdge();


	//
	bool fristCreate = false;
	if ( !mOpening )
	{
		mOpening = true;
		fristCreate = true;
	}

	if ( fristCreate )
	{
		showPanels( true );

		// ����Camera�����¼�
		MfcInputEventMgr::getSingleton().addEvent<CameraFlyEvent>();

		this->UnCheckedToolBarItem( ID_BUTTON_COMPONENT );
		this->UnCheckedToolBarItem( ID_BUTTON_GRID );
		this->UnCheckedToolBarItem( ID_BUTTON_LIGHT );


		m_wndBottomCommDlg.notifySceneCreate();
		m_wndHouseGridDlg.notifySceneCreate();
		m_wndResourceDlg.notifySceneCreate();
		m_wndHouseCompDlg.notifySceneCreate();
	}

	//
	m_wndBottomCommDlg.notifyHouseSceneEnable();

	if ( terrain )
	{
		RootManager::getSingleton().OnTerrainCreate();
		SceneListenerManager::getSingleton()._fireSceneReset();
	}

	if ( house && !fristCreate )	//��������˷���, �����Ǵ�����������
	{
		m_wndHouseGridDlg.notifySceneCreate();
		m_wndHouseCompDlg.notifySceneCreate();
	}
}

void CMainFrame::OnShowPanel()
{
	showPanels( true );
}

void CMainFrame::OnSceneDestroy()
{
	mOpening = false;

	RootManager::getSingleton().getOperatorManager()->reset();
	RootManager::getSingleton().getTerrainGrid()->destroyRenderable();

	m_wndBottomCommDlg.notifySceneDestroy();
	m_wndHouseGridDlg.notifySceneDestroy();
	m_wndResourceDlg.notifySceneDestroy();
	m_wndHouseCompDlg.notifySceneDestroy();
	m_wndSceneObjectDlg.notifySceneDestroy();
	m_wndCPathSearchDlg.notifySceneDestroy();

	//����¼� 
	MfcInputEventMgr::getSingleton().clearEvents();

	showPanels( false );


	//ɾ����Դ��
	if ( mFromOgreMaxScene )
	{
		Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("OgreMax");
		mFromOgreMaxScene = false;
	}
}
void CMainFrame::OnTestAutoFindPath()
{
	if ( !EQSceneManager::getSingleton().getHouseScene() )
		return;
	//�����¼�
	MfcInputEventMgr::getSingleton().addEvent<TestAutoFindPathEvent>();
}

void CMainFrame::OnSetAmbient()
{
	if ( !mOpening )
		return;

	this->ShowControlBar( &m_wndAmbientBar, TRUE, FALSE  );	
	this->m_wndResourceDlg.showAmbientSetting();
}

void CMainFrame::OnSetPostProcess()
{
	if ( !mOpening )
		return;

	this->ShowControlBar( &m_wndPostProcessBar, TRUE, FALSE  );	
	this->m_wndResourceDlg.showPostProcessSetting();
}

void CMainFrame::OnSetLight()
{
	if ( !mOpening )
		return;

	int idx = m_wndToolBar.CommandToIndex( ID_BUTTON_LIGHT );
	UINT val = m_wndToolBar.GetButtonStyle( idx );		
	if ( val & TBBS_CHECKED )
	{
		MfcInputEventMgr::getSingleton().addEvent<SetLightEvent>();

		this->ShowControlBar( &m_wndLightBar, TRUE, FALSE  );			
		this->m_wndResourceDlg.showSelectComponent();
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<SetLightEvent>();
		this->ShowControlBar( &m_wndLightBar, FALSE, FALSE  );
	}
}

////////////////////////////
#include "TerrainData.h"
#include "Terrain.h"

void CMainFrame::OnCreateTerrain()
{
	//����Ҫ����
	//EQSceneManager::getSingleton().getHouseScene()->getSceneInfo()->initialise();
}
void CMainFrame::OnLoadRemoveFarmScene()
{
	if ( mOpening )
	{
		if ( !EQSceneManager::getSingleton().getFarmScene() )
			EQSceneManager::getSingleton().loadFarmScene();
		else
			EQSceneManager::getSingleton().destroyFarmScene();
	}
}

void CMainFrame::OnRunFarmScript()
{
	runFarmScript();
}

void CMainFrame::OnUpdateRunFarmScript(CCmdUI *pCmdUI)
{
	CMenu *pMenu = GetMenu();
	if ( !EQSceneManager::getSingleton().getFarmScene() )
		pMenu->EnableMenuItem(ID_RUN_FARM_SCRIPT,MF_BYCOMMAND| MF_GRAYED); 
	else
		pMenu->EnableMenuItem(ID_RUN_FARM_SCRIPT,MF_BYCOMMAND|MF_ENABLED); 
}

void CMainFrame::OnUpdateLoadRemoveFarmScene(CCmdUI *pCmdUI)
{
	CMenu *pMenu = GetMenu();
	if ( mOpening )
		pMenu->EnableMenuItem(ID_LOAD_REMOVE_FARM_SCENE,MF_BYCOMMAND| MF_ENABLED); 
	else
		pMenu->EnableMenuItem(ID_LOAD_REMOVE_FARM_SCENE,MF_BYCOMMAND| MF_GRAYED); 
}

void CMainFrame::OnEditUndo()
{
	if ( EQSceneManager::getSingleton().getSceneInfo()->isInOctreeScene() )
		return;

	RootManager::getSingleton().getOperatorManager()->undo();
}

void CMainFrame::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	CMenu *pMenu = GetMenu();
	OperatorManager* mgr = RootManager::getSingleton().getOperatorManager();
	if ( mgr && mgr->getUndoOperators().empty() )
		pMenu->EnableMenuItem(ID_EDIT_UNDO,MF_BYCOMMAND| MF_GRAYED); 
	else
		pMenu->EnableMenuItem(ID_EDIT_UNDO,MF_BYCOMMAND| MF_ENABLED); 
}

void CMainFrame::OnEditRedo()
{
	if ( EQSceneManager::getSingleton().getSceneInfo()->isInOctreeScene() )
		return;

	RootManager::getSingleton().getOperatorManager()->redo();
}

void CMainFrame::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	CMenu *pMenu = GetMenu();
	OperatorManager* mgr = RootManager::getSingleton().getOperatorManager();
	if ( mgr && mgr->getRedoOperators().empty() )
		pMenu->EnableMenuItem(ID_EDIT_REDO,MF_BYCOMMAND| MF_GRAYED); 
	else
		pMenu->EnableMenuItem(ID_EDIT_REDO,MF_BYCOMMAND| MF_ENABLED); 
}

void CMainFrame::OnUpdateTerrainShadowmap(CCmdUI *pCmdUI)
{
	CMenu *pMenu = GetMenu();
	if ( EQSceneManager::getSingleton().getSceneInfo()->isTerrainEmpty() )
		pMenu->EnableMenuItem(ID_TERRAIN_SHADOWMAP,MF_BYCOMMAND| MF_GRAYED); 
	else
		pMenu->EnableMenuItem(ID_TERRAIN_SHADOWMAP,MF_BYCOMMAND| MF_ENABLED); 
}

void CMainFrame::OnTerrainShadowmap()
{
	Terrain* pTerrain = EQSceneManager::getSingleton().getTerrain();
	TerrainData* pTerrainData = pTerrain->getData();

	CTerrShadowmap dlg(this);
	if ( dlg.DoModal() == IDOK )
	{
		if (!pTerrainData->mLightmapImage)
			pTerrainData->mLightmapImage = new Ogre::Image;
		*pTerrainData->mLightmapImage = dlg.mLightmapImage;

		pTerrain->setLightmapQuality(dlg.mLightmapQuality);		
		pTerrain->buildGeometry(EQSceneManager::getSingleton().getSceneInfo()->getSceneNode(), true);

	}
}

void CMainFrame::OnProjView()
{
	Ogre::Camera* cam = GraphicsSystem::getSingleton().getCamera();
	if ( cam->getProjectionType() == Ogre::PT_ORTHOGRAPHIC )
		cam->setProjectionType(Ogre::PT_PERSPECTIVE);
	else
	{
		cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		Ogre::Vector3 v = cam->getPosition();
		cam->setOrthoWindowHeight(v.length());
	}

}

void CMainFrame::OnUpdateTestOutsideCut(CCmdUI *pCmdUI)
{
	CMenu *pMenu = GetMenu();
	if ( EQSceneManager::getSingleton().getSceneInfo()->isTerrainEmpty() )
		pMenu->EnableMenuItem(ID_TEST_OUTSIDE_CUT,MF_BYCOMMAND| MF_GRAYED); 
	else
		pMenu->EnableMenuItem(ID_TEST_OUTSIDE_CUT,MF_BYCOMMAND| MF_ENABLED); 
}

void CMainFrame::OnTestOutsideCut()
{
	MfcInputEventMgr::getSingleton().delEvent<OutsideCutEvent>();
	MfcInputEventMgr::getSingleton().addEvent<OutsideCutEvent>();
}

void CMainFrame::OnTerrainShot()
{
	if ( !mOpening )
		return;

	TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();
	if(!terr)
	{
		MessageBox("�޵�����Ϣ", "��ʾ");
		return;
	}

	Ogre::Vector3 p1,p2;

	std::pair<Real, Real> pos;
	pos = terr->junctionToWorld(0, 0);
	p1 = Ogre::Vector3(pos.first, 0, pos.second);
	pos = terr->junctionToWorld(terr->mXSize+1, terr->mZSize+1);
	p2 = Ogre::Vector3(pos.first, 0, pos.second);

	Ogre::SceneManager* sm = GraphicsSystem::getSingleton().getSceneMgr();

	Ogre::FogMode mode = sm->getFogMode();
	Ogre::Real exp = sm->getFogDensity();
	Ogre::Real start = sm->getFogStart();
	Ogre::Real end = sm->getFogEnd();
	Ogre::ColourValue col = sm->getFogColour();
	sm->setFog(Ogre::FOG_NONE, col, exp, start, end);

	Ogre::ManualObject* edge = RootManager::getSingleton().getSceneEdgeLine();
	if(edge)
		edge->setVisible(false);

	Ogre::Camera* cam = sm->createCamera("TerrainShot");

 	Ogre::Vector3 center = (p1+p2)/2;
 	Real len1 = abs(p2.x-p1.x);
 	Real len2 = abs(p2.z-p1.z);

// 	cam->setPosition(center + Ogre::Vector3(0,len1/2,0));
// 	Ogre::Quaternion q;
// 	q.FromAxes(Ogre::Vector3::UNIT_X, Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Vector3::UNIT_Y);
// 	cam->setOrientation(q);
// 	cam->setFOVy(Ogre::Degree(90)); 
// 	cam->setNearClipDistance(5);
// 	cam->setFarClipDistance(len1/2+500);
// 	cam->setAspectRatio(len1/len2);
// 	cam->setProjectionType(Ogre::PT_PERSPECTIVE);

  	cam->setAutoAspectRatio(true);
  	cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	cam->setFOVy(Ogre::Degree(90));
 	cam->setOrthoWindowWidth(len1);
 	cam->setOrthoWindowHeight(len2);
 	Ogre::Quaternion q;
 	q.FromAxes(Ogre::Vector3::UNIT_X, Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Vector3::UNIT_Y);
 	cam->setOrientation(q);
 	cam->setNearClipDistance(5);
 	cam->setFarClipDistance(3000);
 	cam->setPosition(center + Ogre::Vector3(0,2000,0));

	float res = min(4096.0f/terr->getXSize(), 4096.0f/terr->getZSize());
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().createManual("TerrainShot", 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				Ogre::TEX_TYPE_2D, 
				terr->getXSize()*res, terr->getZSize()*res, 1, 0, Ogre::PF_BYTE_RGB, 
				Ogre::TU_RENDERTARGET,
				0,
				false,
				4); //��8��AA�Դ治��-_-;
	Ogre::RenderTexture* rt= tex->getBuffer()->getRenderTarget();

	Ogre::Viewport* vp = rt->addViewport( cam );
	vp->setClearEveryFrame(true);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
	vp->setOverlaysEnabled(false);
	vp->setSkiesEnabled(false);

	rt->update();

	std::string logicName = mLastResFileName;
	if (Ogre::StringUtil::endsWith(mLastResFileName, ".scene"))
		logicName = mLastResFileName.substr(0, mLastResFileName.find("/"));
	rt->writeContentsToFile(logicName+"_map.png");

	MessageBox("��ͼ���", "��ʾ");
	sm->destroyCamera(cam);
	Ogre::TextureManager::getSingleton().remove(tex->getHandle());
	sm->setFog(mode, col, exp, start, end);
	if(edge)
		edge->setVisible(true);
}
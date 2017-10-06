// SceneObjectDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "SceneObjectDlg.h"

#include "UISceneObjectEvent.h"
#include "MfcInputEventMgr.h"
#include "MainFrm.h"
#include "EditorTable.h"
#include "SceneEntityObject.h"
#include "GraphicsTable.h"


CSceneObjectDlg* g_CSceneObjectDlg = NULL;
bool g_check = false;

// CSceneObjectDlg dialog

IMPLEMENT_DYNAMIC(CSceneObjectDlg, CDialog)


CSceneObjectDlg::CSceneObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneObjectDlg::IDD, pParent)
	, mTreeChanging(false)
	, mTransformButtonChanging(false)
{
	g_CSceneObjectDlg = this;
}

CSceneObjectDlg::~CSceneObjectDlg()
{
}

void CSceneObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SO_TREE, mTreeCtl);
	DDX_Control(pDX, IDC_SO_SELECT, mSelectCtl);
	DDX_Control(pDX, IDC_SO_MOVE, mMoveCtl);
	DDX_Control(pDX, IDC_SO_ROTATION, mRotationCtl);
	DDX_Control(pDX, IDC_SO_SCALE, mScaleCtl);
	DDX_Control(pDX, IDC_EDIT1, m_input);
}

void CSceneObjectDlg::notifySystemSetup()
{
	reloadResource();
}

void CSceneObjectDlg::notifySceneDestroy()
{
	MfcInputEventMgr::getSingleton().delEvent<CreateSceneEntityObjectEvent>();
}

bool SceneEntityObjRowMore_(SceneEntityObjRow& a, SceneEntityObjRow& b)
{
	if ( a.mesh < b.mesh )
		return true;
	else if ( a.mesh > b.mesh )
		return false;

	if ( a.material < b.material )
		return true;
	else if ( a.material > b.material )
		return false;

	return false;
}

void CSceneObjectDlg::reloadResource()
{
	mTreeCtl.DeleteAllItems();

#if 0
	HTREEITEM modelHI = mTreeCtl.InsertItem( "模型表现表" );

	const ModelDisplayTable& tab = GraphicsTableManager::getSingleton().getModelDisplayTable();

	for ( ModelDisplayTable::const_iterator it = tab.begin(); it != tab.end(); ++it )
	{
		const ModelDisplayRow& row = it->second;
		std::string str = Ogre::StringConverter::toString(row.id) + "|" + row.mesh;
		mTreeCtl.InsertItem( str.c_str(), modelHI );
	}

	mTreeCtl.SortChildren( modelHI );
#endif

	HTREEITEM modelHI = mTreeCtl.InsertItem( "SceneEntityObj.xml" );

	SceneEntityObjTable& tab = GraphicsTableManager::getSingleton().getSceneEntityObjTable();
	for ( SceneEntityObjTable::const_iterator it = tab.begin(); it != tab.end(); ++it )
	{
		const SceneEntityObjRow& row = *it;
		std::string str = row.mesh;
		if ( !row.material.empty() )
		{
			str += "|";
			str += row.material;
		}
		else
		{
			str += "|<N>";
		}

		if ( row.light != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.light);
		}
		else
			str += "|<N>";

		if ( !row.cutLevel.empty() )
		{
			str += "|";
			str += row.cutLevel;
		}
		else
			str += "|<N>";

		if ( row.particle != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.particle);
		}
		else
			str += "|<N>";

		mTreeCtl.InsertItem( str.c_str(), modelHI );
	}

	HTREEITEM modelHI_id = mTreeCtl.InsertItem( "SceneEntityObj.xml -id" );
	SceneEntityObjMapTable& tab_id_ = GraphicsTableManager::getSingleton().getSceneEntityObjMapTable();
	SceneEntityObjTable tab_id;
	tab_id.reserve(tab_id_.size());
	for ( SceneEntityObjMapTable::const_iterator it = tab_id_.begin(); it != tab_id_.end(); ++it )
		tab_id.push_back(it->second);
	std::sort(tab_id.begin(), tab_id.end(), SceneEntityObjRowMore_);

	for ( SceneEntityObjTable::const_iterator it = tab_id.begin(); it != tab_id.end(); ++it )
	{
		const SceneEntityObjRow& row = *it;

		std::string str = Ogre::StringConverter::toString(row.id);

		str += "|";
		str += row.mesh;

		if ( !row.material.empty() )
		{
			str += "|";
			str += row.material;
		}
		else
		{
			str += "|<N>";
		}

		if ( row.light != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.light);
		}
		else
			str += "|<N>";

		if ( !row.cutLevel.empty() )
		{
			str += "|";
			str += row.cutLevel;
		}
		else
			str += "|<N>";

		if ( row.particle != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.particle);
		}
		else
			str += "|<N>";

		mTreeCtl.InsertItem( str.c_str(), modelHI_id );
	}

	//
	HTREEITEM modelHI2 = mTreeCtl.InsertItem( "模型目录" );

	Ogre::ArchiveManager::ArchiveMapIterator archIt = Ogre::ArchiveManager::getSingleton().getArchiveIterator();

	Ogre::Archive* arch = NULL;
	while ( archIt.hasMoreElements() )
	{
		Ogre::Archive* ar = archIt.getNext();
		if ( ar->getName() == "../../Product/Media/Model" )
		{
			arch = ar;
			break;
		}
	}

	if ( arch )
	{
		Ogre::StringVectorPtr strvecPtr = arch->find("*.mesh");
		std::vector<std::string> strvec = *strvecPtr;
		for ( std::vector<std::string>::iterator it = strvec.begin(); it != strvec.end(); ++it )
		{
			const std::string& name = *it;
			mTreeCtl.InsertItem( name.c_str(), modelHI2 );
		}
	}
}

void CSceneObjectDlg::unselectTree()
{
	if ( !mTreeChanging && !g_MainFrameClosing )
	{
		g_CSceneObjectDlg->mTreeCtl.SelectItem(NULL);
		//顺便切换到选择
		if ( !g_check )
		{
			mSelectCtl.SetCheck(1);
			OnCheck(0,true);
		}
	}
}

void CSceneObjectDlg::changeTransformButton(const std::string& button)
{
	if ( g_MainFrameClosing )
		return;

	mTransformButtonChanging = true;

	mSelectCtl.SetCheck(0);
	mMoveCtl.SetCheck(0);
	mRotationCtl.SetCheck(0);
	mScaleCtl.SetCheck(0);

	if ( button == "select" )
	{
		mSelectCtl.SetCheck(1);
	}
	else if ( button == "move" )
	{
		mMoveCtl.SetCheck(1);
	}
	else if ( button == "rotation" )
	{
		mRotationCtl.SetCheck(1);
	}
	else if ( button == "scale" )
	{
		mScaleCtl.SetCheck(1);
	}
	else
	{
		//nothing
	}

	mTransformButtonChanging = false;
}


BEGIN_MESSAGE_MAP(CSceneObjectDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_SO_TREE, &CSceneObjectDlg::OnTvnSelchangedSoTree)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_SO_MOVE, &CSceneObjectDlg::OnBnClickedSoMove)
	ON_BN_CLICKED(IDC_SO_ROTATION, &CSceneObjectDlg::OnBnClickedSoRotation)
	ON_BN_CLICKED(IDC_SO_SCALE, &CSceneObjectDlg::OnBnClickedSoScale)
	ON_BN_CLICKED(IDC_SO_SELECT, &CSceneObjectDlg::OnBnClickedSoSelect)
	ON_EN_CHANGE(IDC_EDIT1, &CSceneObjectDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CSceneObjectDlg message handlers

void CSceneObjectDlg::OnOK()
{
	//CDialog::OnOK();
}

void CSceneObjectDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CSceneObjectDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if ( mTreeCtl.m_hWnd )
	{
		CRect rc;
		GetClientRect( rc );
		rc.top += 60;
		mTreeCtl.MoveWindow( rc );
	}

}

BOOL CSceneObjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mSelectTT.Create(this,TTS_ALWAYSTIP);
	mSelectTT.Activate(TRUE);
	mSelectTT.AddTool(&mSelectCtl,"选择(Q)");

	mMoveTT.Create(this,TTS_ALWAYSTIP);
	mMoveTT.Activate(TRUE);
	mMoveTT.AddTool(&mMoveCtl,"移动(W) CTRL键贴地移动 SHIFT键垂直移动 ALT网格量化移动");

	mRotationTT.Create(this,TTS_ALWAYSTIP);
	mRotationTT.Activate(TRUE);
	mRotationTT.AddTool(&mRotationCtl,"旋转(E) CTRL键45度倍数旋转");

	mScaleTT.Create(this,TTS_ALWAYSTIP);
	mScaleTT.Activate(TRUE);
	mScaleTT.AddTool(&mScaleCtl,"缩放(R)");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneObjectDlg::OnTvnSelchangedSoTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	mTreeChanging = true;

	//先清除事件
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateSceneEntityObjectEvent>();
	}

	//
	HTREEITEM itemId = mTreeCtl.GetSelectedItem();
	if ( itemId == NULL )
		return;

	HTREEITEM parentId = mTreeCtl.GetParentItem(itemId);
	if ( parentId == NULL )
		return;

	CString parentName = mTreeCtl.GetItemText(parentId);
	
#if 0
	if ( parentName == "模型表现表" )
	{
		CString ss = mTreeCtl.GetItemText(itemId);
		std::string str = ss;
		std::vector< std::string  > strList = Ogre::StringUtil::split(str,"|");
		assert(strList.size() == 2);
		const ModelDisplayRow* row = GraphicsTableManager::getSingleton().getModelDisplayRow( Ogre::StringConverter::parseInt(strList[0]) );
		
		CreateSceneEntityObjectEvent* eve = MfcInputEventMgr::getSingleton().addEvent<CreateSceneEntityObjectEvent>();
		//eve->setResource(row->mesh, row->mat);
	}
#endif
	if ( parentName == "SceneEntityObj.xml" )
	{
		CString ss = mTreeCtl.GetItemText(itemId);
		std::string str = ss;
		std::vector< std::string  > strList = Ogre::StringUtil::split(str,"|");
		assert(strList.size() == 4);
		std::string mesh = strList[0];
		std::string material = "";
		uint light = 0;
		uint particle = 0;
		int cut = CDL_NONE;
		if ( strList[1] != "<N>" )
			material = strList[1];

		if ( strList[2] != "<N>" )
			light = Ogre::StringConverter::parseUnsignedLong( strList[2] );

		if ( strList[3] == "f" )
			cut = CDL_FAR;
		else if ( strList[3] == "m" )
			cut = CDL_MID;
		else if ( strList[3] == "c" )
			cut = CDL_CLOSE;

		if ( strList[4] != "<N>" )
			particle = Ogre::StringConverter::parseUnsignedLong( strList[4] );

		CreateSceneEntityObjectEvent* eve = MfcInputEventMgr::getSingleton().addEvent<CreateSceneEntityObjectEvent>();
		eve->setResource(mesh, material, light, particle, cut);
	}
	else if ( parentName == "SceneEntityObj.xml -id" )
	{
		CString ss = mTreeCtl.GetItemText(itemId);
		std::string str = ss;
		std::vector< std::string  > strList = Ogre::StringUtil::split(str,"|");
		assert(strList.size() > 5);
		uint id = Ogre::StringConverter::parseUnsignedInt(strList[0]);

		CreateSceneEntityObjectEvent* eve = MfcInputEventMgr::getSingleton().addEvent<CreateSceneEntityObjectEvent>();
		eve->setResource(id);
	}
	else if ( parentName == "模型目录" )
	{
		CString ss = mTreeCtl.GetItemText(itemId);
		std::string str = ss;

		CreateSceneEntityObjectEvent* eve = MfcInputEventMgr::getSingleton().addEvent<CreateSceneEntityObjectEvent>();
		eve->setResource(str, "", 0, 0, 0);
	}

	mTreeChanging = false;
}

void CSceneObjectDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if ( !bShow )
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateSceneEntityObjectEvent>();
		MfcInputEventMgr::getSingleton().delEvent<SceneObjectTransform>();
	}
}

void CSceneObjectDlg::OnBnClickedSoMove()
{
	OnCheck(1, (mMoveCtl.GetCheck() == BST_CHECKED));
}

void CSceneObjectDlg::OnBnClickedSoRotation()
{
	OnCheck(2, (mRotationCtl.GetCheck() == BST_CHECKED));
}

void CSceneObjectDlg::OnBnClickedSoScale()
{
	OnCheck(3, (mScaleCtl.GetCheck() == BST_CHECKED));
}

void CSceneObjectDlg::OnBnClickedSoSelect()
{
	OnCheck(0, (mSelectCtl.GetCheck() == BST_CHECKED));
}

void CSceneObjectDlg::OnCheck(int button, bool press)
{
	if ( mTransformButtonChanging )
		return;

	g_check = true;

	if ( !press )
		MfcInputEventMgr::getSingleton().delEvent<SceneObjectTransform>();
	else
	{
		SceneObjectTransform* eve = MfcInputEventMgr::getSingleton().getEvent<SceneObjectTransform>();
		if ( !eve )
		{
			eve = MfcInputEventMgr::getSingleton().addEvent<SceneObjectTransform>();
			eve->initTransMode((SceneObjectTransform::TransMode)button);
		}
		else
		{
			eve->changeTransMode((SceneObjectTransform::TransMode)button);
		}
	}

	g_check =false;
}
BOOL CSceneObjectDlg::PreTranslateMessage(MSG* pMsg)
{
	if(mSelectTT.GetSafeHwnd()!=NULL)
		mSelectTT.RelayEvent(pMsg);

	if(mMoveTT.GetSafeHwnd()!=NULL)
		mMoveTT.RelayEvent(pMsg);

	if(mRotationTT.GetSafeHwnd()!=NULL)
		mRotationTT.RelayEvent(pMsg);

	if(mScaleTT.GetSafeHwnd()!=NULL)
		mScaleTT.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CSceneObjectDlg::OnEnChangeEdit1()
{
	CString str;
	m_input.GetWindowText(str);
	std::string input(str.GetBuffer());

	mTreeCtl.DeleteAllItems();

	HTREEITEM modelHI = mTreeCtl.InsertItem( "SceneEntityObj.xml" );

	SceneEntityObjTable& tab = GraphicsTableManager::getSingleton().getSceneEntityObjTable();
	for ( SceneEntityObjTable::const_iterator it = tab.begin(); it != tab.end(); ++it )
	{
		const SceneEntityObjRow& row = *it;

		if(!input.empty())
		{
			if(row.mesh.find(input) == std::string::npos)
				continue;
		}

		std::string str = row.mesh;
		if ( !row.material.empty() )
		{
			str += "|";
			str += row.material;
		}
		else
		{
			str += "|<N>";
		}

		if ( row.light != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.light);
		}
		else
			str += "|<N>";

		if ( !row.cutLevel.empty() )
		{
			str += "|";
			str += row.cutLevel;
		}
		else
			str += "|<N>";

		if ( row.particle != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.particle);
		}
		else
			str += "|<N>";

		mTreeCtl.InsertItem( str.c_str(), modelHI );
	}
	if(!input.empty())
	{
		mTreeCtl.Expand(modelHI, TVE_EXPAND);
	}

	HTREEITEM modelHI_id = mTreeCtl.InsertItem( "SceneEntityObj.xml -id" );
	SceneEntityObjMapTable& tab_id_ = GraphicsTableManager::getSingleton().getSceneEntityObjMapTable();
	SceneEntityObjTable tab_id;
	tab_id.reserve(tab_id_.size());
	for ( SceneEntityObjMapTable::const_iterator it = tab_id_.begin(); it != tab_id_.end(); ++it )
		tab_id.push_back(it->second);
	std::sort(tab_id.begin(), tab_id.end(), SceneEntityObjRowMore_);

	for ( SceneEntityObjTable::const_iterator it = tab_id.begin(); it != tab_id.end(); ++it )
	{
		const SceneEntityObjRow& row = *it;

		if(!input.empty())
		{
			if(row.mesh.find(input) == std::string::npos)
				continue;
		}

		std::string str = Ogre::StringConverter::toString(row.id);

		str += "|";
		str += row.mesh;

		if ( !row.material.empty() )
		{
			str += "|";
			str += row.material;
		}
		else
		{
			str += "|<N>";
		}

		if ( row.light != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.light);
		}
		else
			str += "|<N>";

		if ( !row.cutLevel.empty() )
		{
			str += "|";
			str += row.cutLevel;
		}
		else
			str += "|<N>";

		if ( row.particle != 0 )
		{
			str += "|";
			str += Ogre::StringConverter::toString(row.particle);
		}
		else
			str += "|<N>";

		mTreeCtl.InsertItem( str.c_str(), modelHI_id );
	}
	if(!input.empty())
	{
		mTreeCtl.Expand(modelHI_id, TVE_EXPAND);
	}

	//
	HTREEITEM modelHI2 = mTreeCtl.InsertItem( "模型目录" );

	Ogre::ArchiveManager::ArchiveMapIterator archIt = Ogre::ArchiveManager::getSingleton().getArchiveIterator();

	Ogre::Archive* arch = NULL;
	while ( archIt.hasMoreElements() )
	{
		Ogre::Archive* ar = archIt.getNext();
		if ( ar->getName() == "../../Product/Media/Model" )
		{
			arch = ar;
			break;
		}
	}

	if ( arch )
	{
		Ogre::StringVectorPtr strvecPtr = arch->find("*.mesh");
		std::vector<std::string> strvec = *strvecPtr;
		for ( std::vector<std::string>::iterator it = strvec.begin(); it != strvec.end(); ++it )
		{
			const std::string& name = *it;

			if(!input.empty())
			{
				if(name.find(input) == std::string::npos)
					continue;
			}

			mTreeCtl.InsertItem( name.c_str(), modelHI2 );
		}
		if(!input.empty())
		{
			mTreeCtl.Expand(modelHI2, TVE_EXPAND);
		}
	}
}

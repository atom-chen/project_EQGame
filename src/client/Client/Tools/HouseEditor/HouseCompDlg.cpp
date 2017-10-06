// HouseCompDlg.cpp : implementation file
//


#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "HouseCompDlg.h"
#include "HouseTable.h"
#include "HouseSceneComponent.h"
#include "HSCManager.h"
#include "MainFrm.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "MfcInputEventMgr.h"
#include "CommFunc.h"
#include "EQLog.h"
#include "UIFitmentEvent.h"


CHouseCompDlg * g_HouseCompDlg = NULL;

BEGIN_MESSAGE_MAP(CHouseCompDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, &CHouseCompDlg::OnNMSetfocusTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CHouseCompDlg::OnNMClickTree1)
	ON_CBN_SELCHANGE(IDC_PUTMODE, &CHouseCompDlg::OnCbnSelchangePutmode)
	ON_EN_CHANGE(IDC_FIND, &CHouseCompDlg::OnEditChange)
END_MESSAGE_MAP()




CHouseCompDlg::CHouseCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHouseCompDlg::IDD, pParent)
{
	g_HouseCompDlg = this;
}

CHouseCompDlg::~CHouseCompDlg()
{
}

void CHouseCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndHouseCompTree);
	DDX_Control(pDX, IDC_PUTMODE, m_cmbPutMode);
	DDX_Control(pDX, IDC_FIND, m_input);
}


void CHouseCompDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if ( m_wndHouseCompTree.m_hWnd )
	{
		CRect rc;
		GetClientRect( rc );
		// 预留空位，供摆放按钮
		rc.top += 60;
		
		m_wndHouseCompTree.MoveWindow( rc );		
	}
}

void CHouseCompDlg::initTree()
{
	m_wndHouseCompTree.DeleteAllItems();

	std::map<int, HTREEITEM> vclass;
	std::map<uint, int> mlist;

	//读取分类信息
	Ogre::ConfigFile cf;
	cf.load("../Config/furni_classify.cfg");
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName;
	std::vector<std::string> strs;
	HTREEITEM tree1,tree2,tree3,other;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;

		if(secName.empty())
			continue;

		if(secName != "家具")
		{
			tree1 = m_wndHouseCompTree.InsertItem( secName.c_str() );

			for (i = settings->begin(); i != settings->end(); ++i)
			{
				tree2 = m_wndHouseCompTree.InsertItem( i->first.c_str(), tree1 );
				int index = Ogre::StringConverter::parseUnsignedInt(i->second);
				vclass[index] = tree2;
			}
		}
		else
		{
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				int id = Ogre::StringConverter::parseUnsignedInt(i->first);
				int cl = Ogre::StringConverter::parseUnsignedInt(i->second);
				mlist[id] = cl;
			}
		}
	}
	other = m_wndHouseCompTree.InsertItem( "未分类" );
	
	for ( HouseCompTable::const_iterator it = HouseTableManager::getSingleton().getHouseCompTable().begin();
		it != HouseTableManager::getSingleton().getHouseCompTable().end();
		++it )
	{	
		uint ID = it->second.ID;
		//主动光和穿透光不加入
		if(ID==13 || ID==14)
			continue;

		std::map<uint, int>::iterator ite = mlist.find(ID);
		tree3 = m_wndHouseCompTree.InsertItem( it->second.Name.c_str(), ite!=mlist.end()? vclass[ite->second] : other );
		m_wndHouseCompTree.SetItemData( tree3, ID );
	}
	
	//for(iteclass=mapclass.begin(); iteclass!=mapclass.end(); iteclass++)
	//	m_wndHouseCompTree.Expand(iteclass->second, TVE_EXPAND);
	//m_wndHouseCompTree.Expand(other, TVE_EXPAND);
}


void CHouseCompDlg::notifySystemSetup()
{
	initTree();
	m_cmbPutMode.SetCurSel( 0 );
}

void CHouseCompDlg::notifySceneCreate()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce && sce->isLoadHouse() )
	{//enable all ctrl
		int id_num;
		CWnd* pChildWnd = this->GetWindow(GW_CHILD);
		while(pChildWnd != NULL)
		{  
			id_num=pChildWnd->GetDlgCtrlID();
			pChildWnd->EnableWindow(true);
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
		}
	}
	else
	{//disable all ctrl
		int id_num;
		CWnd* pChildWnd = this->GetWindow(GW_CHILD);
		while(pChildWnd != NULL)
		{  
			id_num=pChildWnd->GetDlgCtrlID();
			pChildWnd->EnableWindow(false);
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
		}
	}
}

void CHouseCompDlg::notifySceneDestroy()
{
	{//disable all ctrl
		int id_num;
		CWnd* pChildWnd = this->GetWindow(GW_CHILD);
		while(pChildWnd != NULL)
		{  
			id_num=pChildWnd->GetDlgCtrlID();
			pChildWnd->EnableWindow(false);
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
		}
	}
}

void CHouseCompDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if ( bShow )
	{
		if ( EQSceneManager::getSingleton().getHouseScene() )
		{
			// 切到家装模式
			//g_BottomCommDlg->changeGridMode(GD_MODE_FITMENT);
			MfcInputEventMgr::getSingleton().addEvent<EditFitmentEvent>();
		}
	}
	else // 处理隐藏
	{	
		MfcInputEventMgr::getSingleton().delEvent<EditFitmentEvent>();
	}

	CDialog::OnShowWindow(bShow, nStatus);
}

void CHouseCompDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CHouseCompDlg::OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	g_ChildView->SetFocus();
}

void CHouseCompDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	EditFitmentEvent * eve = MfcInputEventMgr::getSingleton().addEvent<EditFitmentEvent>();

	CPoint point; 
	UINT pflags; 
	GetCursorPos(&point); 
	m_wndHouseCompTree.ScreenToClient( &point ); 
	HTREEITEM hitItem = m_wndHouseCompTree.HitTest( point, &pflags ); 	

	if ( !hitItem )
		return;

	if ( !m_wndHouseCompTree.GetParentItem(hitItem) )
		return;

	uint compID = m_wndHouseCompTree.GetItemData(hitItem);

	eve->createNewFitment( compID );
}

void CHouseCompDlg::OnCbnSelchangePutmode()
{
	int idx = m_cmbPutMode.GetCurSel();

	HouseScene* scene = EQSceneManager::getSingleton().getHouseScene();
	assert( scene );

	scene->getFitmentCtrl().setPutMode( static_cast<HSCFitmentCtrl::PutMode>(idx) );

	g_ChildView->SetFocus();
}

void CHouseCompDlg::OnEditChange()
{
	CString str;
	m_input.GetWindowText(str);
	std::string input(str.GetBuffer());
	
	m_wndHouseCompTree.DeleteAllItems();

	std::map<int, HTREEITEM> vclass;
	std::map<uint, int> mlist;

	//读取分类信息
	Ogre::ConfigFile cf;
	cf.load("../Config/furni_classify.cfg");
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName;
	std::vector<std::string> strs;
	HTREEITEM tree1,tree2,tree3,other;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;

		if(secName.empty())
			continue;

		if(secName != "家具")
		{
			tree1 = m_wndHouseCompTree.InsertItem( secName.c_str() );

			for (i = settings->begin(); i != settings->end(); ++i)
			{
				tree2 = m_wndHouseCompTree.InsertItem( i->first.c_str(), tree1 );
				int index = Ogre::StringConverter::parseUnsignedInt(i->second);
				vclass[index] = tree2;
			}
		}
		else
		{
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				int id = Ogre::StringConverter::parseUnsignedInt(i->first);
				int cl = Ogre::StringConverter::parseUnsignedInt(i->second);
				mlist[id] = cl;
			}
		}
	}
	other = m_wndHouseCompTree.InsertItem( "未分类" );

	for ( HouseCompTable::const_iterator it = HouseTableManager::getSingleton().getHouseCompTable().begin();
		it != HouseTableManager::getSingleton().getHouseCompTable().end();
		++it )
	{	
		uint ID = it->second.ID;
		//主动光和穿透光不加入
		if(ID==13 || ID==14)
			continue;

		if(!input.empty())
		{
			if (Ogre::StringConverter::isNumber(input))
			{
				if(it->second.ID != Ogre::StringConverter::parseInt(input))
					continue;
			}
			else
			{
				if(it->second.Name.find(input) == std::string::npos)
					continue;
			}
		}

		std::map<uint, int>::iterator ite = mlist.find(ID);
		HTREEITEM item = ite!=mlist.end()? vclass[ite->second] : other;
		tree3 = m_wndHouseCompTree.InsertItem( it->second.Name.c_str(), item );
		m_wndHouseCompTree.SetItemData( tree3, ID );

		if(!input.empty())
		{
			m_wndHouseCompTree.Expand(m_wndHouseCompTree.GetParentItem(item), TVE_EXPAND);
			m_wndHouseCompTree.Expand(item, TVE_EXPAND);
		}
	}
}

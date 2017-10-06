// ModelDlg.cpp : implementation file
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"
#include "RoleRaceDlg.h"

#include "AvatarTable.h"
#include "EQMain.h"
#include "Avatar.h"
#include "AvatarAdjuster.h"
#include "AnimationDlg.h"
#include "MainFrm.h"
#include "ItemPack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const int EQUIP_SLOT_COUNT = 8; // 8 ��װ����λ

BEGIN_MESSAGE_MAP(CRoleRaceDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MO_TREE, &CRoleRaceDlg::OnTvnSelchangedMoTree)
	ON_WM_SIZE()
	ON_NOTIFY(NM_SETFOCUS, IDC_MO_TREE, &CRoleRaceDlg::OnNMSetfocusMoTree)
END_MESSAGE_MAP()


void CRoleRaceDlg::GraphListener::notifySystemSetup()
{
	CRoleRaceDlg & dlg = *pOwner;

	const RoleRaceTable & tb = AvatarTableManager::getSingleton().getRoleRaceTable();

	// ��ɫ�ڵ�
	::HTREEITEM hAvatar = dlg.m_wndRoleTree.InsertItem( "��ɫ" );

	// Ů�ڵ�
	::HTREEITEM hGirl = dlg.m_wndRoleTree.InsertItem( "Ů", hAvatar );
	
	// �нڵ�
	::HTREEITEM hBoy = dlg.m_wndRoleTree.InsertItem( "��", hAvatar );

	uint roleID = 0;
	// ����Ů������֧,��ӽڵ�
	for ( RoleRaceTable::const_iterator it = tb.begin();
		it != tb.end();
		++it )
	{	
		roleID = it->second.id;
		if ( GT_MALE == it->second.gender )	// ��
		{
			HTREEITEM h = dlg.m_wndRoleTree.InsertItem( it->second.skeleton.c_str(), hBoy );
			dlg.m_wndRoleTree.SetItemData( h, roleID );			
		}
		else							// Ů
		{
			HTREEITEM h = dlg.m_wndRoleTree.InsertItem( it->second.skeleton.c_str(), hGirl );
			dlg.m_wndRoleTree.SetItemData( h, roleID );			
		}		
	}

	dlg.m_wndRoleTree.SortChildren( hBoy );
	dlg.m_wndRoleTree.SortChildren( hGirl );
	// ȫ��չ��
	dlg.m_wndRoleTree.Expand( hAvatar, TVE_EXPAND );
	dlg.m_wndRoleTree.Expand( hBoy, TVE_EXPAND );
	dlg.m_wndRoleTree.Expand( hGirl, TVE_EXPAND );
	
	//// ����Ĭ�Ͻ�ɫ	
	//static_cast<CMainFrame *>(::AfxGetMainWnd())->getModelDlg().setRoleRace( roleID );
}



CRoleRaceDlg::CRoleRaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoleRaceDlg::IDD, pParent)
{
	m_graphLis.setOwner(this);

	GraphicsSystem::getSingleton().addListener(&m_graphLis);
}

CRoleRaceDlg::~CRoleRaceDlg()
{
	GraphicsSystem::getSingleton().removeListener(&m_graphLis);
}

void CRoleRaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MO_TREE, m_wndRoleTree);
}



void CRoleRaceDlg::setRoleRace( uint roleID )
{
	if ( 0 == roleID )
		return;

	::CMainFrame * frm = dynamic_cast<CMainFrame *>( ::AfxGetMainWnd() );

	// ����ǰ
	frm->doBeforeAvatarCreate();	

	if ( g_Avatar )
		delete g_Avatar;
	if ( g_ItemPack )
	{
		ItemPackLogic::clearItemPack(*g_ItemPack);
		delete g_ItemPack;
	}
	// �����µĽ�ɫ����
	g_Avatar = new Avatar( roleID );
	g_ItemPack = new ItemPack();
	g_ItemPack->init(EQUIP_SLOT_COUNT);

	// ������
	frm->doAfterAvatarCreate();		
}

void CRoleRaceDlg::OnTvnSelchangedMoTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	ulong roleID =  pNMTreeView->itemNew.lParam;
	
	setRoleRace( roleID );
}

void CRoleRaceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if ( m_wndRoleTree.m_hWnd )
	{
		CRect rc;
		GetClientRect( rc );
		m_wndRoleTree.MoveWindow( rc );
	}
}

void CRoleRaceDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CRoleRaceDlg::OnNMSetfocusMoTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	static_cast<CMainFrame *>( ::AfxGetMainWnd() )->m_wnd3DView.SetFocus();
}

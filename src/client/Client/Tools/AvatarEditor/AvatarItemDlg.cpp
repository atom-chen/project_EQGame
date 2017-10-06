// AvatarSubDlg.cpp : implementation file
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"
#include "AvatarItemDlg.h"
#include "AvatarTable.h"
#include "ItemTable.h"
#include "Avatar.h"
#include "MainFrm.h"
#include "Item.h"
#include "ItemPack.h"
#include "CommFunc.h"
#include "EQException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static std::vector<string> GetSmallItemTypesDesc( uchar sex, uint bigType )
{
	typedef std::vector<SmallTypesRow> TableType;
	const TableType & tab = ItemTableManager::getSingleton().getSmallTypesTable();
	for ( TableType::const_iterator it = tab.begin(); it != tab.end(); ++it )
	{
		if ( it->sex == sex && it->big_type == bigType )
			return it->small_types;
	}
	return std::vector<string>();
}


static std::vector<string> GetManItemDesc( uint bigType )
{
	return GetSmallItemTypesDesc(GT_MALE, bigType);
	/*std::vector<string> arr;
	if ( bigType == 1 )
	{
		arr.push_back("��װ");
		arr.push_back("��װ");
		arr.push_back("���");
		arr.push_back("��̨װ");
		arr.push_back("����");
		arr.push_back("��װ");
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("˯��");
		arr.push_back("��ͨ˯��");
		arr.push_back("����װ");
		arr.push_back("����װ");
		arr.push_back("����װ");
		arr.push_back("�����˶�װ");
		arr.push_back("Ӿװ");
		arr.push_back("����2");
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("��ʷ");
	}
	else if ( bigType == 2 )
	{
		arr.push_back("T��");
		arr.push_back("����");
		arr.push_back("���/�����");
		arr.push_back("����");
		arr.push_back("Ƥ��");
		arr.push_back("����");
		arr.push_back("ë��");
		arr.push_back("ţ��");
	}
	else if ( bigType == 3 )
	{
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("ɳ̲");
		arr.push_back("�̿�");
		arr.push_back("ţ��");
	}
	else if ( bigType == 4 )
	{
		arr.push_back("ţ��ñ");
		arr.push_back("Ѽ��ñ");
		arr.push_back("��֯ñ");
		arr.push_back("����ñ");
		arr.push_back("����");
	}
	else if ( bigType == 5 )
	{
		arr.push_back("����");
		arr.push_back("���");
		arr.push_back("����");
	}
	else if ( bigType == 6 )
	{
		arr.push_back("�۾�");
	}
	else if ( bigType == 7 )
	{
		arr.push_back("�ֳ�");
	}
	else if ( bigType == 8 )
	{
		arr.push_back("����");
	}
	else if ( bigType == 9 )
	{
		arr.push_back("ƤЬ");
		arr.push_back("��Ь");
		arr.push_back("��Ь");
		arr.push_back("�˶�Ь");
		arr.push_back("ѥ");
	}
	return arr;*/
}

static std::vector<string> GetWomanItemDesc( uint bigType )
{
	return GetSmallItemTypesDesc(GT_FEMALE, bigType);
	/*std::vector<string> arr;
	if ( bigType == 1 )
	{
		arr.push_back("��װ");
		arr.push_back("��װ");
		arr.push_back("���");
		arr.push_back("��̨װ");
		arr.push_back("����");
		arr.push_back("��װ");
		arr.push_back("����ȹ");
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("˯��");
		arr.push_back("��ͨ˯��");
		arr.push_back("����װ");
		arr.push_back("����װ");
		arr.push_back("����װ");
		arr.push_back("�����˶�װ");
		arr.push_back("Ӿװ");
		arr.push_back("����2");
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("��ʷ");
	}
	else if ( bigType == 2 )
	{
		arr.push_back("T��");
		arr.push_back("����");
		arr.push_back("Ĩ�ص�");
		arr.push_back("���/�����");
		arr.push_back("����");
		arr.push_back("Ƥ��");
		arr.push_back("����");
		arr.push_back("ë��");
		arr.push_back("ţ��");
	}
	else if ( bigType == 3 )
	{
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("ɳ̲");
		arr.push_back("�̿�");
		arr.push_back("ţ��");
		arr.push_back("ȹ��");
		arr.push_back("����");
	}
	else if ( bigType == 4 )
	{
		arr.push_back("ţ��ñ");
		arr.push_back("Ѽ��ñ");
		arr.push_back("��֯ñ");
		arr.push_back("����ñ");
		arr.push_back("����");
	}
	else if ( bigType == 5 )
	{
		arr.push_back("����");
		arr.push_back("����");
		arr.push_back("���");
	}
	else if ( bigType == 6 )
	{
		arr.push_back("�۾�");
	}
	else if ( bigType == 7 )
	{
		arr.push_back("�ֳ�");
	}
	else if ( bigType == 8 )
	{
		arr.push_back("����");
	}
	else if ( bigType == 9 )
	{
		arr.push_back("ƤЬ");
		arr.push_back("��Ь");
		arr.push_back("��Ь");
		arr.push_back("�˶�Ь");
		arr.push_back("ѥ");
	}
	return arr;*/
}

static ItemEquipBigType slotToBigType( uint slot )
{
	if ( slot == ES_HAT )
		return IEBT_HAT;
	if ( slot == ES_COAT )
		return IEBT_COAT;
	if ( slot == ES_NOSE )
		return IEBT_NOSE;
	if ( slot == ES_PANTS )
		return IEBT_PANTS;
	if ( slot == ES_BACK )
		return IEBT_BACK;
	if ( slot == ES_SHOES )
		return IEBT_SHOES;
	if ( slot == ES_HAND )
		return IEBT_HAND;
	if ( slot == ES_ORNAMENT )
		return IEBT_ORNAMENT;
	return IEBT_NONE;
}


BEGIN_MESSAGE_MAP(CAvatarItemDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAvatarItemDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_AVS_TAKEOFF, &CAvatarItemDlg::OnBnClickedAvsTakeoff)
	ON_LBN_DBLCLK(IDC_AVS_ITEMLIST, &CAvatarItemDlg::OnLbnDblclkAvsItemlist)
	ON_LBN_SELCHANGE(IDC_AVS_ITEMLIST, &CAvatarItemDlg::OnLbnSelchangeAvsItemlist)
	ON_BN_CLICKED(IDC_BUTTON1, &CAvatarItemDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CAvatarItemDlg::OnEnChangeEdit1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CAvatarItemDlg::OnNMClickTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CAvatarItemDlg::OnNMDblclkTree1)
END_MESSAGE_MAP()


CAvatarItemDlg::CAvatarItemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAvatarItemDlg::IDD, pParent)
	, mCurrBigType(IEBT_NONE)
{
}

CAvatarItemDlg::~CAvatarItemDlg()
{
}

void CAvatarItemDlg::resetContent()
{
	// ������� �������Ʒ
	GraphicsSystem::getSingleton().displayItem( 0 );

	m_wndItemList.ResetContent();
	m_typeTree.DeleteAllItems();	
	m_edtSearch.SetWindowText("");
}

void CAvatarItemDlg::showItems( ItemEquipBigType bigType )
{
	loadItems( bigType );
	ShowWindow( SW_SHOW );
}

void CAvatarItemDlg::reloadItems()
{
	loadItems( mCurrBigType );	
}

void CAvatarItemDlg::loadItems( ItemEquipBigType bigType )
{
	resetContent();

	mCurrBigType = bigType;

	switch( bigType )
	{
	case IEBT_SUIT:
		SetWindowText("��װ");
		break;
	case IEBT_HAT:
		SetWindowText("ñ��");
		break;
	case IEBT_COAT:
		SetWindowText("����");
		break;
	case IEBT_PANTS:
		SetWindowText("����");
		break;
	case IEBT_SHOES:
		SetWindowText("�Ų�");
		break;
	case IEBT_NOSE:
		SetWindowText("�۾�");
		break;
	case IEBT_BACK:
		SetWindowText("����/����");
		break;
	case IEBT_HAND:
		SetWindowText("�ֳ�");
		break;
	case IEBT_ORNAMENT:
		SetWindowText("����");
		break;
	default:
		{
			SetWindowText("");
			mCurrBigType = IEBT_NONE;
			return;
		}
		break;
	};

	if ( !g_Avatar )
		return;


	std::vector<string> arr;
	if (g_Avatar->getRoleRaceInfo()->gender == GT_MALE)
		arr = ::GetManItemDesc(mCurrBigType);
	else
		arr = ::GetWomanItemDesc(mCurrBigType);
	
	for ( size_t i = 0; i < arr.size(); ++i )
	{
		::HTREEITEM hItem =	m_typeTree.InsertItem(arr[i].c_str());
		m_typeTree.SetCheck(hItem, TRUE);
	}

	clearSearch();
}

void CAvatarItemDlg::addItem( ulong id, const std::string& name )
{
	std::string str = "[" + Ogre::StringConverter::toString(id,8,'0') + "] ";
	str += name;
	m_wndItemList.SetItemData(m_wndItemList.AddString(str.c_str()), id);
}

void CAvatarItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVS_ITEMLIST, m_wndItemList);
	DDX_Control(pDX, IDC_BUTTON1, m_btnClear);
	DDX_Control(pDX, IDC_EDIT1, m_edtSearch);
	DDX_Control(pDX, IDC_TREE1, m_typeTree);
}

void CAvatarItemDlg::OnBnClickedOk()
{
	doSelectItem();
	OnOK();
}

void CAvatarItemDlg::OnCancel()
{
	resetContent();
	CDialog::OnCancel();
}

void CAvatarItemDlg::OnOK()
{
	resetContent();
	CDialog::OnOK();
}

/** ���ж�°�ť
*/
void CAvatarItemDlg::OnBnClickedAvsTakeoff()
{
	if ( g_Avatar )
	{
		std::vector<const ItemEquipRow *> v;
		ItemTableManager::getSingleton().getBigTypeCollect( mCurrBigType, v );
		if (!v.empty())
		{
			const ItemEquipRow * pItem = v.front();
			innerTakeOff(pItem->equip_placeholder);
		}
	}
}

void CAvatarItemDlg::OnLbnDblclkAvsItemlist()
{
	doSelectItem();
}

void CAvatarItemDlg::doSelectItem()
{
	if ( !g_Avatar )
		return;

	int sel = m_wndItemList.GetCurSel();
	if ( sel < 0 )
		return;

	ulong ix = m_wndItemList.GetItemData( sel );

	const ItemEquipRow * pItem = ItemTableManager::getSingleton().getItemEquipRow( ix );
	if ( pItem )
	{
		// �ж��ܷ�װ����Ʒ�����߼�Ӧ������һ��	
		bool canEquipItem = true;
		if ( canEquipItem )
		{
			// ȡ�������µ���ز�λ
			uint takeOffSlots = ItemPackLogic::getTakeOffSlots(*g_ItemPack, pItem->equip_placeholder );

			innerTakeOff( takeOffSlots );
			innerEquipOn( pItem->equip_placeholder, *pItem );
		}
	}
}

void CAvatarItemDlg::OnLbnSelchangeAvsItemlist()
{
	// ��ʾ��ǰ���ڿ�����Ʒ	
	int sel = m_wndItemList.GetCurSel();
	
	if ( sel < 0 )
		return;

	ulong ix = m_wndItemList.GetItemData( sel );
	
	std::vector<const ModelDisplayRow *> rows;
	ItemTableManager::getSingleton().getItemModels( ix, rows );
	GraphicsSystem::getSingleton().displayItem( rows );
}

void CAvatarItemDlg::OnBnClickedButton1()
{
	clearSearch();
}

void CAvatarItemDlg::clearSearch()
{
	GraphicsSystem::getSingleton().displayItem( 0 );
	m_edtSearch.SetWindowText("");
}

void CAvatarItemDlg::filterItems( const std::vector<uint> & smallTypes, const std::string & containsStr )
{
	// �����Ʒ�б�
	m_wndItemList.ResetContent();

	if (!g_Avatar)
		return;

	if (smallTypes.empty())
		return;

	// ����������Ʒ�б�
	std::vector<const ItemEquipRow *> v;
	ItemTableManager::getSingleton().getBigTypeCollect( mCurrBigType, v );
	for ( size_t i = 0; i < v.size(); ++i )
	{		
		const ItemEquipRow * r = v[i];

		// �жϽ�ɫ�ɷ�ʹ����Ʒ, ����߼�Ӧ������һ��
		bool canUseItem = (r->sex == 0) || (r->sex == g_Avatar->getRoleRaceInfo()->gender);
		if ( !canUseItem )
			continue;

		// �������С�����У�������
		if ( std::find(smallTypes.begin(), smallTypes.end(), r->type2) == smallTypes.end() )
			continue;

		// �����ִ�
		if (!containsStr.empty())
		{
			std::string sID = Ogre::StringConverter::toString(r->id, 8, '0');
			bool bFindID = (sID.find(containsStr, 0) < sID.size());
			bool bFindName = (r->name.find(containsStr, 0) < r->name.size());
			// ģ��ƥ��id �� name, ������Ҳ����������� 
			if (!bFindID && !bFindName)
				continue;
		}

		addItem( r->id, r->name );		
	}		
}

void CAvatarItemDlg::OnEnChangeEdit1()
{
	filterItems( getSmallTypes(), getFilterStr() );
}

std::string CAvatarItemDlg::getFilterStr() const
{
	CString str;
	m_edtSearch.GetWindowText(str);
	return str.GetBuffer();
}

std::vector<uint> CAvatarItemDlg::getSmallTypes() const
{
	std::vector<uint> arr;
	uint smallType = 1;
	for ( HTREEITEM hItem = m_typeTree.GetRootItem();	
		NULL != hItem;
		hItem = m_typeTree.GetNextSiblingItem(hItem), ++smallType )
	{
		if ( !m_typeTree.GetCheck(hItem) )
			continue;

		arr.push_back(smallType);
	}

	return arr;
}

// mouse click
void CAvatarItemDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ���Խ��
	LRESULT ignore_result = 1;
	*pResult = ignore_result;

	CPoint pt;
	::GetCursorPos(&pt);
	m_typeTree.ScreenToClient(&pt);

	::HTREEITEM hItem = m_typeTree.HitTest(pt);
	if ( hItem )
	{
		if ( m_typeTree.GetCheck(hItem) )
		{
			m_typeTree.SetCheck(hItem, FALSE);
		}
		else
			m_typeTree.SetCheck(hItem, TRUE);
	}

	filterItems( getSmallTypes(), getFilterStr() );
}

// mouse Double click
void CAvatarItemDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ���Խ��
	LRESULT ignore_result = 1;
	*pResult = ignore_result;

	CPoint pt;
	::GetCursorPos(&pt);
	m_typeTree.ScreenToClient(&pt);

	::HTREEITEM hHit = m_typeTree.HitTest(pt);
	if ( hHit )
	{
		// ȫ����Ϊ��ѡunchecked
		for ( HTREEITEM hItem = m_typeTree.GetRootItem();	
			NULL != hItem;
			hItem = m_typeTree.GetNextSiblingItem(hItem) )
		{
			m_typeTree.SetCheck(hItem, FALSE);
		}

		// ֻ����һ��Ϊcheck
		m_typeTree.SetCheck(hHit, TRUE);
	}

	filterItems( getSmallTypes(), getFilterStr() );
}


void CAvatarItemDlg::innerTakeOff( uint takeOffSlots )
{
	std::vector<uint> slots = UIntToBitsetArray( takeOffSlots );
	if ( !slots.empty() )
	{
		::CMainFrame * pWin = dynamic_cast<CMainFrame *>( ::AfxGetMainWnd() );
		// ж��������ز�λ��װ��
		for ( uint i = 0; i < slots.size(); ++i )
		{
			g_Avatar->takeOff( slots[i] );
			// ȥ��������ʾ
			pWin->m_wndRoleSectionDlg.setLabelText( slotToBigType(slots[i]), "" );	

			uint idx = UIntToBit( slots[i] );
			g_ItemPack->setItem(idx, 0, 0, 0, 0);
		}		

		if ( (takeOffSlots & ES_COAT) != 0 &&
			(takeOffSlots & ES_PANTS) != 0 )
		{
			// ȥ����װ����
			pWin->m_wndRoleSectionDlg.setLabelText( IEBT_SUIT, "" );	
		}
	}
}

static uint getAItemID(uint equipRowID)
{
	// ͨ��id ȡ��һ��item
	for ( ItemTable::const_iterator it = ItemTableManager::getSingleton().getItemTable().begin();
		it != ItemTableManager::getSingleton().getItemTable().end();
		++it )
	{
		if (it->second.subID == equipRowID && it->second.type == IT_EQUIPMENT)
			return it->second.id;
	}

	return 0;
}

void CAvatarItemDlg::innerEquipOn( uint equipOnSlots, const ItemEquipRow & item )
{
	std::vector<uint> slots = UIntToBitsetArray( equipOnSlots );

	if ( slots.empty() )
		return;
	// �ٴ��ڵ�һ����λ��
	g_Avatar->equipOn( slots.front(), item.equip_disp_id, item.equip_placeholder );
	uint idx = ::UIntToBit( slots.front() );

	
	g_ItemPack->setItem( idx, 0, getAItemID(item.id), 1, 0 );
	
	::CMainFrame * pWin = dynamic_cast<CMainFrame *>( ::AfxGetMainWnd() );

	std::string strName = "[" + Ogre::StringConverter::toString(item.id, 8, '0') + "] ";
	strName += item.name;
	pWin->m_wndRoleSectionDlg.setLabelText( mCurrBigType, strName );	
}

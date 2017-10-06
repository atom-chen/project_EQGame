// HouseGridDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "HouseGridDlg.h"
#include "SceneManager.h"
#include "CommFuncGcs.h"
#include "SystemGcs.h"
#include "MfcInputEventMgr.h"
#include "MainFrm.h"
#include "HouseTable.h"
#include "EQException.h"
#include "HouseScene.h"


CHouseGridDlg * g_HouseGridDlg = NULL;

IMPLEMENT_DYNAMIC(CHouseGridDlg, CDialog)

BEGIN_MESSAGE_MAP(CHouseGridDlg, CDialog)
	ON_BN_CLICKED(IDC_HG_ASSPLA, &CHouseGridDlg::OnBnClickedHgAsspla)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_HG_FLOORHEI_SLD, &CHouseGridDlg::OnNMReleasedcaptureHgFloorheiSld)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_HG_CREATEGRID, &CHouseGridDlg::OnBnClickedHgCreategrid)
	ON_BN_CLICKED(IDC_HG_WALLGRID, &CHouseGridDlg::OnBnClickedHgWallgrid)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_HG_STAIRGRID, &CHouseGridDlg::OnBnClickedHgStairGrid)
	ON_EN_CHANGE(IDC_HG_FLOORHEI, &CHouseGridDlg::OnEnChangeHgFloorhei)
	ON_BN_CLICKED(IDC_HG_EDITROOM, &CHouseGridDlg::OnBnClickedHgEditroom)
	ON_CBN_SELCHANGE(IDC_HG_ROOMID, &CHouseGridDlg::OnCbnSelchangeHgRoomno)
	ON_EN_CHANGE(IDC_HG_ASSPLA_GRID, &CHouseGridDlg::OnEnChangeHgAssplaGrid)
END_MESSAGE_MAP()



CHouseGridDlg::CHouseGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHouseGridDlg::IDD, pParent)
	, mLastFloorHeightSldPos(0)
{
	g_HouseGridDlg = this;
}

CHouseGridDlg::~CHouseGridDlg()
{
}

void CHouseGridDlg::resetContent()
{
	{ //��ǰ¥��
		m_wndAssistPlane.SetCheck(BST_UNCHECKED);
		m_wndAssistPlaneGridNum.SetWindowText("10");
		m_wndFloorHeight.SetWindowText("0");

		m_wndFloorHeightSld.SetRange(-50,50);
		m_wndFloorHeightSld.SetTicFreq(10);
		//m_wndFloorHeightSld.SetPos(0);
		m_wndFloorHeightSld.SetPageSize(10);
		m_wndFloorHeightSld.SetLineSize(10);

		_resetFloorHeightSld();
	}
}
void CHouseGridDlg::enableAllCtrl(bool enable)
{
	int id_num;
	CWnd* pChildWnd = this->GetWindow(GW_CHILD);
	while(pChildWnd != NULL)
	{  
		id_num=pChildWnd->GetDlgCtrlID();
		pChildWnd->EnableWindow(enable);
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CHouseGridDlg::notifySceneCreate()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce && sce->isLoadHouse() )
	{
		enableAllCtrl(true);
		resetContent();
	}
	else
		enableAllCtrl(false);
}

void CHouseGridDlg::notifySceneDestroy()
{
	//resetContent();
	enableAllCtrl(false);
}

void CHouseGridDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if ( bShow ) // ��ʾ����
	{
		//g_BottomCommDlg->changeGridMode(GD_MODE_STYLE);
	}
	else // ���ش���
	{			
		//m_wndAssistPlane.SetCheck(BST_UNCHECKED);
	}

	CDialog::OnShowWindow(bShow, nStatus);
}

void CHouseGridDlg::notifySystemSetup()
{
	{
		//��������
		m_wndGridType.ResetContent();

		const HouseGridTypeTable& tab = HouseTableManager::getSingleton().getHouseGridTypeTable();
		HouseGridTypeTable::const_iterator it;
		int ix = 0;
		for ( it = tab.begin(); it != tab.end(); ++it )
		{
			const HouseGridTypeRow& row = it->second;;
			if ( row.TypeID > HOUSE_VISIBLE_GRID_TYPE )
			{
				std::string str = row.Name + "  " + Ogre::StringConverter::toString(row.TypeID);
				m_wndGridType.InsertString(ix,str.c_str());
				++ix;
			}
		}

		if ( ix > 0 )
			m_wndGridType.SetCurSel(0);
		else
		{
			MessageBox("HouseGridType.xml û���û�����");
			EQ_EXCEPT( "HouseGridType.xml û���û�����!", "CHouseGridDlg::OnInitDialog" );
		}
	}

	m_wndRoomID.ResetContent();
	int i;
	m_wndRoomID.InsertString(0, "ȫ��");
	m_wndRoomID.SetItemData(0, 99);
	for(i=0; i<=20; i++)
	{
		m_wndRoomID.InsertString(i+1, Ogre::StringConverter::toString(i).c_str());
		m_wndRoomID.SetItemData(i+1, i);
	}
}

void CHouseGridDlg::_resetFloorHeightSld()
{
	mLastFloorHeightSldPos = 0;
	m_wndFloorHeightSld.SetPos(0);
}

void CHouseGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HG_ASSPLA, m_wndAssistPlane);
	DDX_Control(pDX, IDC_HG_ASSPLA_GRID, m_wndAssistPlaneGridNum);
	DDX_Control(pDX, IDC_HG_FLOORHEI, m_wndFloorHeight);
	DDX_Control(pDX, IDC_HG_FLOORHEI_SLD, m_wndFloorHeightSld);
	DDX_Control(pDX, IDC_HG_CREATEGRID, m_wndCreateGrid);
	DDX_Control(pDX, IDC_HG_WALLGRID, m_wndWallGrid);
	DDX_Control(pDX, IDC_HG_GRIDTYPE, m_wndGridType);
	DDX_Control(pDX, IDC_HG_STAIRGRID, m_wndLinkGrid);
	DDX_Control(pDX, IDC_HG_EDITROOM, m_wndEditRoom);
	DDX_Control(pDX, IDC_HG_ROOMID, m_wndRoomID);
}


void CHouseGridDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CHouseGridDlg::OnBnClickedHgAsspla()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( !sce )
		return;

	sce->showAssistPlane( m_wndAssistPlane.GetCheck() == BST_CHECKED );
}


void CHouseGridDlg::OnOK()
{
	if ( &m_wndFloorHeight == GetFocus() )
	{
		SetFocus();
	}

	//CDialog::OnOK();
}


void CHouseGridDlg::OnNMReleasedcaptureHgFloorheiSld(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	_resetFloorHeightSld();
}

void CHouseGridDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if ( (CSliderCtrl*)pScrollBar == &m_wndFloorHeightSld )
	{
		int curr = m_wndFloorHeightSld.GetPos();//ȡ�õ�ǰλ��ֵ

		int delta = curr - mLastFloorHeightSldPos;

		if ( delta != 0 )
		{
			CString mstr;
			m_wndFloorHeight.GetWindowText(mstr);
			int height = Ogre::StringConverter::parseInt(mstr.GetBuffer());

			height += delta;
			m_wndFloorHeight.SetWindowText(Ogre::StringConverter::toString(height).c_str());
		}

		mLastFloorHeightSldPos = curr;
	}
}

void CHouseGridDlg::setFloorHeight( int height )
{
	m_wndFloorHeight.SetWindowText(Ogre::StringConverter::toString(height).c_str());
	mLastFloorHeightSldPos = height;
}

void CHouseGridDlg::OnBnClickedHgCreategrid()
{
	if ( m_wndCreateGrid.GetCheck() == BST_CHECKED )
	{
		//�����¼�
		MfcInputEventMgr::getSingleton().addEvent<CreateGridEvent>();

		//����������ʾ
		if ( g_BottomCommDlg->getGridDisplayMode() < GD_MODE_STYLE )
			g_BottomCommDlg->changeGridMode(GD_MODE_STYLE);
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateGridEvent>();
	}
}

void CHouseGridDlg::OnBnClickedHgWallgrid()
{
	if ( m_wndWallGrid.GetCheck() == BST_CHECKED )
	{
		//�����¼�
		MfcInputEventMgr::getSingleton().addEvent<WallGridEvent>();

		//����������ʾ
		if ( g_BottomCommDlg->getGridDisplayMode() < GD_MODE_CONNECTEDNESS )
			g_BottomCommDlg->changeGridMode(GD_MODE_CONNECTEDNESS);
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<WallGridEvent>();
	}
}

ushort CHouseGridDlg::getGridTypeID() const
{
	CString cstr;
	m_wndGridType.GetWindowText(cstr);

	std::vector<std::string> strVec = Ogre::StringUtil::split(cstr.GetBuffer());
	assert(strVec.size()>0);
	return (ushort)Ogre::StringConverter::parseUnsignedLong(strVec[strVec.size()-1]);
}

void CHouseGridDlg::OnBnClickedHgStairGrid()
{
	if ( m_wndLinkGrid.GetCheck() == BST_CHECKED )
	{
		//�����¼�
		MfcInputEventMgr::getSingleton().addEvent<CreateLinkGridEvent>();

		//����������ʾ
		if ( g_BottomCommDlg->getGridDisplayMode() < GD_MODE_CONNECTEDNESS )
			g_BottomCommDlg->changeGridMode(GD_MODE_CONNECTEDNESS);
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<CreateLinkGridEvent>();
	}	
}

void CHouseGridDlg::OnEnChangeHgFloorhei()
{
	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	assert( sce );

	HouseSceneFloor* floor = sce->getWalkViewFloor();
	assert( floor );

	CString mstr;
	m_wndFloorHeight.GetWindowText(mstr);

	int height = Ogre::StringConverter::parseInt(mstr.GetBuffer());

	if ( height != floor->getFloorHeight() )
	{
		if ( floor->hasGrids() )
		{
			m_wndFloorHeight.SetWindowText(Ogre::StringConverter::toString(floor->getFloorHeight()).c_str());
			//MessageBox("�������, ������ɾ���������������!", "��ʾ");
			g_MainFrame->showError( "�������, ������ɾ���������������!" );
		}
		else
		{
			floor->modifyFloorHeight(height);			
		}
	}
}

void CHouseGridDlg::OnBnClickedHgEditroom()
{
	// TODO: Add your control notification handler code here
	if ( m_wndEditRoom.GetCheck() == BST_CHECKED )
	{
		//�����¼�
		MfcInputEventMgr::getSingleton().addEvent<EditRoomEvent>();

		//����������ʾ
		if ( g_BottomCommDlg->getGridDisplayMode() != GD_MODE_CONNECTEDNESS )
			g_BottomCommDlg->changeGridMode(GD_MODE_CONNECTEDNESS);

		m_wndRoomID.EnableWindow(TRUE);
		m_wndRoomID.SetCurSel(0);
	}
	else
	{
		MfcInputEventMgr::getSingleton().delEvent<EditRoomEvent>();

		m_wndRoomID.EnableWindow(FALSE);
	}
}

void CHouseGridDlg::OnCbnSelchangeHgRoomno()
{
	// TODO: Add your control notification handler code here
	if(m_wndEditRoom.GetCheck() != BST_CHECKED)
		return;

	int roomid = m_wndRoomID.GetItemData(m_wndRoomID.GetCurSel());

	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	assert( sce );
	HouseSceneFloor* floor = sce->getWalkViewFloor();
	assert( floor );

	floor->filterByRoomID(roomid);
}

void CHouseGridDlg::OnEnChangeHgAssplaGrid()
{
	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	assert( sce );

	CString sNum;
	m_wndAssistPlaneGridNum.GetWindowText(sNum);
	int gridNum = Ogre::StringConverter::parseInt(sNum.GetBuffer());

	bool bOk = (0 < gridNum && gridNum <= 100);

	if ( bOk )
		sce->setFloorAssistGridNum( gridNum );	
}

int CHouseGridDlg::getRoomID() const
{
	int roomID = m_wndRoomID.GetItemData( m_wndRoomID.GetCurSel() );
	return roomID;
}

#include "HouseEditorPCH.h"
#include "resource.h"
#include "BottomCommDlg.h"
#include "GraphicsSystem.h"
#include "SceneManager.h"
#include "MainFrm.h"
#include "MfcInputEventMgr.h"
#include "HouseScene.h"
#include "RootManager.h"
#include "TerrainGrid.h"
#include "SystemGcs.h"

CBottomCommDlg * g_BottomCommDlg = NULL;

IMPLEMENT_DYNAMIC(CBottomCommDlg, CDialog)

BEGIN_MESSAGE_MAP(CBottomCommDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BC_HOUSE_UP, &CBottomCommDlg::OnBnClickedBcHouseUp)
	ON_BN_CLICKED(IDC_BC_HOUSE_DOWN, &CBottomCommDlg::OnBnClickedBcHouseDown)
	ON_CBN_SELCHANGE(IDC_BC_HOUSE_MODE, &CBottomCommDlg::OnCbnSelchangeBcHouseMode)
	ON_BN_CLICKED(IDC_BC_CAM_WORLD_GRID, &CBottomCommDlg::OnBnClickedBcCamWorldGrid)
	ON_CBN_SELCHANGE(IDC_BC_GRID_MODE, &CBottomCommDlg::OnCbnSelchangeBcGridMode)
	ON_CBN_SELCHANGE(IDC_BC_BRUSH_TYPE, &CBottomCommDlg::OnCbnSelchangeBcBrushType)
	ON_EN_KILLFOCUS(IDC_BC_BRUSH_SIZE_X, &CBottomCommDlg::OnEnKillfocusBcBrushSizeX)
	ON_EN_KILLFOCUS(IDC_BC_BRUSH_SIZE_Z, &CBottomCommDlg::OnEnKillfocusBcBrushSizeZ)
	ON_EN_CHANGE(IDC_BC_BRUSH_SIZE_X, &CBottomCommDlg::OnEnChangeBcBrushSizeX)
	ON_EN_CHANGE(IDC_BC_BRUSH_SIZE_Z, &CBottomCommDlg::OnEnChangeBcBrushSizeZ)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BC_BRUSH_SPIN_X, &CBottomCommDlg::OnDeltaposBcBrushSpinX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BC_BRUSH_SPIN_Z, &CBottomCommDlg::OnDeltaposBcBrushSpinZ)
	ON_BN_CLICKED(IDC_BC_CAM_TERRGRID, &CBottomCommDlg::OnBnClickedBcCamTerrgrid)
	ON_BN_CLICKED(IDC_BC_CAM_EDGE, &CBottomCommDlg::OnBnClickedBcCamEdge)
END_MESSAGE_MAP()

CBottomCommDlg::CBottomCommDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBottomCommDlg::IDD, pParent)
	, mBrushSizeXInt(0)
	, mBrushSizeZInt(0)
{
	g_BottomCommDlg = this;
}

CBottomCommDlg::~CBottomCommDlg()
{
}

void CBottomCommDlg::resetContent()
{

	{ //摄像机
		m_wndCamRate.SetWindowText("1.0");

		m_wndCamRateSld.SetRange(0,100);
		m_wndCamRateSld.SetTicFreq(10);
		m_wndCamRateSld.SetPos(50);
		m_wndCamRateSld.SetPageSize(10);
		m_wndCamRateSld.SetLineSize(10);
	}

	{ // 世界网格
		m_wndWorldGrid.SetCheck(BST_CHECKED);
		Ogre::ManualObject* grid = GraphicsSystem::getSingleton().getWorldGrid();
		if ( grid )
		{
			if ( grid->getVisible() )
				m_wndWorldGrid.SetCheck(BST_CHECKED);
			else
				m_wndWorldGrid.SetCheck(BST_UNCHECKED);
		}
	}

	{ //房屋
		m_wndHouseMode.ResetContent();
		m_wndHouseMode.InsertString(0,"隐藏");
		m_wndHouseMode.InsertString(1,"无墙");
		m_wndHouseMode.InsertString(2,"半墙");
		m_wndHouseMode.InsertString(3,"全墙");
		m_wndHouseMode.InsertString(4,"屋顶");
		m_wndHouseMode.SetCurSel(2);

		m_wndHouseCurrFloor.SetWindowText("0");
	}

	{ //网格显示
		m_wndGridMode.ResetContent();
		m_wndGridMode.InsertString(0,"普通");
		m_wndGridMode.InsertString(1,"家装");
		m_wndGridMode.InsertString(2,"编辑");
		m_wndGridMode.InsertString(3,"连通性");
		m_wndGridMode.SetCurSel(0);
	}

	{ //笔刷
		size_t x = RootManager::getSingleton().getBrushXSize();
		size_t z = RootManager::getSingleton().getBrushZSize();
		mBrushSizeXCtl.SetWindowText(Ogre::StringConverter::toString(x).c_str());
		mBrushSizeZCtl.SetWindowText(Ogre::StringConverter::toString(x).c_str());
		mBrushTypeCtl.ResetContent();
		mBrushTypeCtl.InsertString(0,"方形画刷");
		mBrushTypeCtl.InsertString(1,"椭圆形画刷");
		mBrushTypeCtl.SetCurSel(0);
		mBrushSpinXCtl.SetBuddy(&mBrushSizeXCtl);
		mBrushSpinXCtl.SetRange(1, 9);
		mBrushSpinXCtl.SetPos(x);
		mBrushSpinZCtl.SetBuddy(&mBrushSizeZCtl);
		mBrushSpinZCtl.SetRange(1, 9);
		mBrushSpinZCtl.SetPos(z);
		RootManager::getSingleton().setBrush("BoxBrushShape", x, z);
	}
}

void CBottomCommDlg::changeGridMode(int mode)
{
	if ( !m_wndGridMode.m_hWnd )
		return;
	int sel = m_wndGridMode.GetCurSel();
	if ( sel == mode - 1 )
		return;

	m_wndGridMode.SetCurSel(mode-1);

	OnCbnSelchangeBcGridMode();
}

void CBottomCommDlg::notifySceneCreate()
{
	resetContent();
}

void CBottomCommDlg::notifySceneDestroy()
{
	resetContent();
}

void CBottomCommDlg::notifyHouseSceneEnable()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce && sce->isLoadHouse() )
	{
		GetDlgItem( IDC_BC_HOUSE_UP )->EnableWindow(TRUE);
		GetDlgItem( IDC_BC_HOUSE_DOWN )->EnableWindow(TRUE);
		m_wndHouseMode.EnableWindow(TRUE);
		m_wndGridMode.EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem( IDC_BC_HOUSE_UP )->EnableWindow(FALSE);
		GetDlgItem( IDC_BC_HOUSE_DOWN )->EnableWindow(FALSE);
		m_wndHouseMode.EnableWindow(FALSE);
		m_wndGridMode.EnableWindow(FALSE);
	}
}

void CBottomCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BC_CAM_RATE, m_wndCamRate);
	DDX_Control(pDX, IDC_BC_CAM_RATESLD, m_wndCamRateSld);
	DDX_Control(pDX, IDC_BC_HOUSE_MODE, m_wndHouseMode);
	DDX_Control(pDX, IDC_BC_HOUSE_CURRFLOOR, m_wndHouseCurrFloor);
	DDX_Control(pDX, IDC_BC_CAM_WORLD_GRID, m_wndWorldGrid);
	DDX_Control(pDX, IDC_BC_GRID_MODE, m_wndGridMode);
	DDX_Control(pDX, IDC_BC_BRUSH_TYPE, mBrushTypeCtl);
	DDX_Control(pDX, IDC_BC_BRUSH_SIZE_X, mBrushSizeXCtl);
	DDX_Control(pDX, IDC_BC_BRUSH_SIZE_Z, mBrushSizeZCtl);
	DDX_Control(pDX, IDC_BC_BRUSH_SPIN_X, mBrushSpinXCtl);
	DDX_Control(pDX, IDC_BC_BRUSH_SPIN_Z, mBrushSpinZCtl);
	DDX_Text(pDX, IDC_BC_BRUSH_SIZE_X, mBrushSizeXInt);
	DDX_Text(pDX, IDC_BC_BRUSH_SIZE_Z, mBrushSizeZInt);
	DDX_Control(pDX, IDC_BC_CAM_TERRGRID, m_wndTerrGrid);
}

void CBottomCommDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CBottomCommDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if ( (CSliderCtrl*)pScrollBar == &m_wndCamRateSld )
	{
		int curr = m_wndCamRateSld.GetPos();//取得当前位置值
		float rate = Ogre::Math::Pow(10, curr/50.0f)/10.0f;

		std::string str = Ogre::StringConverter::toString(rate,4);
		m_wndCamRate.SetWindowText(str.c_str());

		CameraFlyEvent * eve = MfcInputEventMgr::getSingleton().getEvent<CameraFlyEvent>();
		if ( eve )
		{
			eve->notifyCameraRateChange( rate );
		}
	}
}

void CBottomCommDlg::OnBnClickedBcHouseUp()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	if ( sce )
	{
		bool b = sce->upView();
		HouseSceneFloor* floor = sce->getWalkViewFloor();
		if ( floor )
		{
			int n = floor->getFloorNO();
			m_wndHouseCurrFloor.SetWindowText(Ogre::StringConverter::toString(n).c_str());
			g_HouseGridDlg->setFloorHeight( floor->getFloorHeight() );
		}
	}
}

void CBottomCommDlg::OnBnClickedBcHouseDown()
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert( sce );

	sce->downView();
	HouseSceneFloor* floor = sce->getWalkViewFloor();
	if ( floor )
	{
		int n = floor->getFloorNO();
		m_wndHouseCurrFloor.SetWindowText(Ogre::StringConverter::toString(n).c_str());
		g_HouseGridDlg->setFloorHeight( floor->getFloorHeight() );
	}
}

void CBottomCommDlg::OnCbnSelchangeBcHouseMode()
{
	int sel = m_wndHouseMode.GetCurSel();
	if ( sel < 0 )
		return;

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert( sce );
	sce->changeWatchMode((HouseWatchMode)(sel));
}


void CBottomCommDlg::OnBnClickedBcCamWorldGrid()
{
	int chk = m_wndWorldGrid.GetCheck();
	GraphicsSystem::getSingleton().getWorldGrid()->setVisible( chk == BST_CHECKED );
}

void CBottomCommDlg::OnOK()
{
	//CDialog::OnOK();
	if ( &mBrushSizeXCtl == GetFocus() )
	{
		SetFocus();
	}
	else if ( &mBrushSizeZCtl == GetFocus() )
	{
		SetFocus();
	}
}

void CBottomCommDlg::OnCbnSelchangeBcGridMode()
{
	int sel = m_wndGridMode.GetCurSel();
	if ( sel < 0 )
		return;

	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	assert ( sce );
	sce->changeGridDisplayMode((GridDisplayMode)(sel+1));
}

GridDisplayMode CBottomCommDlg::getGridDisplayMode() const
{
	return static_cast<GridDisplayMode>(m_wndGridMode.GetCurSel() + 1);
}
void CBottomCommDlg::OnCbnSelchangeBcBrushType()
{
	// TODO: Add your control notification handler code here
	int sel = mBrushTypeCtl.GetCurSel();

	if ( sel == 0 )
	{
		RootManager::getSingleton().setBrush("BoxBrushShape",
			RootManager::getSingleton().getBrushXSize(), RootManager::getSingleton().getBrushZSize());
	}
	else if ( sel == 1 )
	{
		RootManager::getSingleton().setBrush("EllipseBrushShape",
			RootManager::getSingleton().getBrushXSize(), RootManager::getSingleton().getBrushZSize());
	}
}

void CBottomCommDlg::OnEnKillfocusBcBrushSizeX()
{
	// TODO: Add your control notification handler code here
	CString mstr;
	mBrushSizeXCtl.GetWindowText(mstr);
	int x = Ogre::StringConverter::parseInt(mstr.GetBuffer());
	int nx = x;

	if ( nx < 1 )
		nx = 1;
	if ( nx > 9 )
		nx = 9;

	if ( x != nx )
		mBrushSizeXInt = nx;
		//mBrushSizeXCtl.SetWindowText(Ogre::StringConverter::toString(nx).c_str());

	if ( mBrushSpinXCtl.GetPos() != nx )
		mBrushSpinXCtl.SetPos(nx);

	RootManager::getSingleton().setBrushXSize(nx);
}

void CBottomCommDlg::OnEnKillfocusBcBrushSizeZ()
{
	// TODO: Add your control notification handler code here
	CString mstr;
	mBrushSizeZCtl.GetWindowText(mstr);
	int z = Ogre::StringConverter::parseInt(mstr.GetBuffer());
	int nz = z;

	if ( nz < 1 )
		nz = 1;
	if ( nz > 9 )
		nz = 9;

	if ( z != nz )
		mBrushSizeZInt = nz;
		//mBrushSizeZCtl.SetWindowText(Ogre::StringConverter::toString(nz).c_str());

	if ( mBrushSpinZCtl.GetPos() != nz )
		mBrushSpinZCtl.SetPos(nz);

	RootManager::getSingleton().setBrushZSize(nz);

}

void CBottomCommDlg::OnEnChangeBcBrushSizeX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString mstr;
	mBrushSizeXCtl.GetWindowText(mstr);
	int x = Ogre::StringConverter::parseInt(mstr.GetBuffer());

	if ( x < 1 || x > 9)
		return;

	mBrushSizeXInt = mBrushSpinXCtl.GetPos();
	RootManager::getSingleton().setBrushXSize(x);
}

void CBottomCommDlg::OnEnChangeBcBrushSizeZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString mstr;
	mBrushSizeZCtl.GetWindowText(mstr);
	int z = Ogre::StringConverter::parseInt(mstr.GetBuffer());

	if ( z < 1 || z > 9)
		return;

	mBrushSizeZInt = mBrushSpinZCtl.GetPos();
	RootManager::getSingleton().setBrushZSize(z);
}

void CBottomCommDlg::OnDeltaposBcBrushSpinX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CBottomCommDlg::OnDeltaposBcBrushSpinZ(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CBottomCommDlg::OnBnClickedBcCamTerrgrid()
{
	bool che;
	if ( m_wndTerrGrid.GetCheck() == 1 )
		che = true;
	else
		che = false;

	RootManager::getSingleton().getTerrainGrid()->setActive(che);
}

void CBottomCommDlg::OnBnClickedBcCamEdge()
{
	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();

	static bool bb = true;
	if ( mgr->hasSceneNode("--SceneEdgeLineNode--") )
	{
		Ogre::SceneNode* node = mgr->getSceneNode("--SceneEdgeLineNode--");
		bb = !bb;
		node->setVisible(bb);
	}
}

// CreateTerrainDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "CreateTerrainDlg.h"


// CCreateTerrainDlg dialog

IMPLEMENT_DYNAMIC(CCreateTerrainDlg, CDialog)

CCreateTerrainDlg::CCreateTerrainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateTerrainDlg::IDD, pParent)
{

}

CCreateTerrainDlg::~CCreateTerrainDlg()
{
}

void CCreateTerrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CT_WIDTH, mWidthCtl);
	DDX_Control(pDX, IDC_CT_DEPTH, mDepthCtl);
}


BEGIN_MESSAGE_MAP(CCreateTerrainDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCreateTerrainDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateTerrainDlg message handlers

BOOL CCreateTerrainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mWidthCtl.SetWindowText("64");
	mDepthCtl.SetWindowText("64");


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateTerrainDlg::OnBnClickedOk()
{
	CString strWidth;
	mWidthCtl.GetWindowText(strWidth);
	CString strDepth;
	mDepthCtl.GetWindowText(strDepth);

	mWidth = Ogre::StringConverter::parseInt(strWidth.GetBuffer());
	mDepth = Ogre::StringConverter::parseInt(strDepth.GetBuffer());
	if ( mWidth < 1 || mWidth > 200 || mDepth < 1 || mDepth > 200 )
	{
		::MessageBox(NULL, "场景大小范围(1~200)", "输入错误", 0);
		return;
	}

	OnOK();
}

// SaveDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "SaveDlg.h"


// CSaveDlg dialog

IMPLEMENT_DYNAMIC(CSaveDlg, CDialog)

CSaveDlg::CSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
	, m_input(_T(""))
{

}

CSaveDlg::~CSaveDlg()
{
}

void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OSEC_RESNAME, m_wndResName);
	DDX_Text(pDX, IDC_OSEC_RESNAME, m_input);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()




void CSaveDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);	
	this->m_wndResName.SetFocus();
	this->SetWindowText( m_caption );
}

std::string CSaveDlg::Execute( const std::string & caption, const std::string & defInputText )
{
	CSaveDlg box;
	box.m_input = defInputText.c_str();
	box.m_caption = caption.c_str();

	if ( IDOK != box.DoModal() )
	{
		return "";
	}

	if ( box.m_input == "" )
	{
		box.MessageBox( "场景名不能为空", "提示" );
	}

	return box.m_input.GetBuffer();
}

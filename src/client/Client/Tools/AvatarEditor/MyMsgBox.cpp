// MyMsgBox.cpp : implementation file
//

#include "AvatarEditorPCH.h"
#include "MyMsgBox.h"

BEGIN_MESSAGE_MAP(CMyMsgBox, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyMsgBox::OnBnClickedButtonOK)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

CMyMsgBox::CMyMsgBox(CWnd* pParent /*=NULL*/)
	: CDialog(CMyMsgBox::IDD, pParent)
{

}

CMyMsgBox::~CMyMsgBox()
{
}

void CMyMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mEdit1);
}


void CMyMsgBox::OnBnClickedButtonOK()
{
	OnCancel();
}

void CMyMsgBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if ( mIcon )
		this->SetIcon( mIcon, false );
	this->SetWindowTextA( mCaption.c_str() );
	this->mEdit1.SetWindowTextA( mMsg.c_str() );

	CDialog::OnShowWindow(bShow, nStatus);
}

void CMyMsgBox::Show( const std::string & msg )
{
	typedef std::vector<std::string> TStrings;
	TStrings strs  = Ogre::StringUtil::split( msg, "\r\n" );

	std::string result;
	for ( TStrings::iterator it = strs.begin(); it != strs.end(); ++it )
	{
		result += *it + "\r\n";
	}
	
	CMyMsgBox box;
	
	box.mMsg = result;
	box.mIcon = ::AfxGetMainWnd()->GetIcon( false );
	CString s;
	::AfxGetMainWnd()->GetWindowTextA( s );
	box.mCaption = s.GetBuffer();
	box.DoModal();
}

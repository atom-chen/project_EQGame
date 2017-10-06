#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "OpenDlg.h"
#include "MainFrm.h"
#include "HouseTable.h"
#include "direct.h"
#include "io.h"

IMPLEMENT_DYNAMIC(COpenDlg, CDialog)

BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &COpenDlg::OnBnClicked)
	ON_LBN_DBLCLK(IDC_OPENLIST, &COpenDlg::OnListDblClicked)
END_MESSAGE_MAP()

COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
: CDialog(IDD_OPEN, pParent)
{

}

COpenDlg::~COpenDlg()
{
}

void subFlodSearch(CString name, std::vector<std::string>& nameList)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile("../../Product/Media/Scene/" + name + "/*.scene");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString n = name + "/" + (LPCTSTR)finder.GetFileName();
		nameList.push_back(n.GetBuffer());
	}
}

void sceneSearch(std::vector<std::string>& nameList)
{
	nameList.clear();

	CFileFind finder;
	BOOL bWorking = finder.FindFile("../../Product/Media/Scene/*.*");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if ( finder.IsDirectory() )
		{
			subFlodSearch(finder.GetFileName(), nameList);
		}
	} 
}

void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENLIST, m_list);
	DDX_Control(pDX, IDOK, m_btn);
}

void COpenDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if(bShow)
	{
		sceneSearch(nNameList);

		int i=0;
		m_list.ResetContent();
		for ( std::vector<std::string>::iterator it = nNameList.begin(); it != nNameList.end(); ++it )
		{
			std::string name = *it;
			m_list.InsertString(i, name.c_str());
			i++;
		}

		//m_list.ResetContent();
		//const PlayerHouseTable talbe = HouseTableManager::getSingleton().getPlayerHouseTable();
		//PlayerHouseTable::const_iterator ite;
		//int i=0;
		//for(ite=talbe.begin(); ite!=talbe.end(); ite++)
		//{
		//	m_list.InsertString(i, ite->second.name.c_str());
		//	m_list.SetItemData(i, ite->second.id);
		//	i++;
		//}
	}
}

void COpenDlg::OnListDblClicked()
{
	OnBnClicked();
}

void COpenDlg::OnBnClicked()
{
	int row = m_list.GetCurSel();
	if(row < 0)
		return;

	CString name;
	m_list.GetText(row, name);
	g_MainFrame->OpenScene(name.GetBuffer());

	//int id = m_list.GetItemData(row);
	//g_MainFrame->OpenScene(HouseTableManager::getSingleton().getPlayerHouseRow(id)->res);

	PostMessage(WM_CLOSE);
}

void COpenDlg::Execute()
{
	COpenDlg dlg;
	if(dlg.DoModal() != IDOK)
		return;
}
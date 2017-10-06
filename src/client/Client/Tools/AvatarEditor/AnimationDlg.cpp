// AnimationDlg.cpp : implementation file
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"
#include "AnimationDlg.h"
#include "EQmain.h"
#include "Avatar.h"
#include "AvatarTable.h"
// CAnimationDlg dialog
BEGIN_MESSAGE_MAP(CAnimationDlg, CDialog)
	ON_BN_CLICKED(IDC_AN_PLAY, &CAnimationDlg::OnBnClickedAnPlay)
	ON_CBN_SELCHANGE(IDC_AN_ACTIONLIST, &CAnimationDlg::OnCbnSelchangeAnActionlist)
	ON_BN_CLICKED(IDC_AN_PAUSE, &CAnimationDlg::OnBnClickedAnPause)
	ON_BN_CLICKED(IDC_AN_STOP, &CAnimationDlg::OnBnClickedAnStop)
	ON_BN_CLICKED(IDC_AN_LOOP, &CAnimationDlg::OnBnClickedAnLoop)
	ON_BN_CLICKED(IDC_AN_RESET, &CAnimationDlg::OnBnClickedAnReset)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

CAnimationDlg::CAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationDlg::IDD, pParent)
{

}

CAnimationDlg::~CAnimationDlg()
{

}

void CAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AN_ACTIONLIST, m_wndAnimCB);
	DDX_Control(pDX, IDC_AN_LOOP, m_wndAnimCheck);
	DDX_Control(pDX, IDC_AN_RATE_SLIDER, m_wndRateSlider);
	DDX_Control(pDX, IDC_AN_RATE_TEXT, m_wndRateText);
}




void CAnimationDlg::resetContent()
{
	m_wndAnimCB.ResetContent();

	m_wndAnimCheck.SetCheck(BST_CHECKED);

	m_wndRateSlider.SetRange(0,40);
	m_wndRateSlider.SetTicFreq(10);
	m_wndRateSlider.SetPos(10);
	m_wndRateSlider.SetPageSize(10);
	m_wndRateSlider.SetLineSize(10);

	m_wndRateText.SetWindowText("速率: 1.0x");

	mActionMap.clear();
}

void CAnimationDlg::notifyAvatarCreate()
{
	typedef std::vector<std::string> StringArray;
	// 处理动画相关
	ModelGcs * mod = g_Avatar->getModel();
	assert( mod );
	std::string skeleton = g_Avatar->getRoleRaceInfo()->skeleton;
	StringArray descArr;
	StringArray actionArr;
	AvatarTableManager::getSingleton().getSkelActions(skeleton, actionArr);
	AvatarTableManager::getSingleton().getSkelActionDescs(skeleton, descArr);

	for ( StringArray::iterator it = actionArr.begin();
		it != actionArr.end();
		++it )
	{
		//uint posStart = it->find_last_of('/') + 1;
		//uint posEnd = it->find_last_of('.');
		//if ( posEnd > posStart )
		//	*it = std::string(it->c_str() + posStart, it->c_str() + posEnd); 
		//else
		//	*it = "";

		//这样写不太合适, 不过在编辑器里面, 无所谓拉, 如果游戏内获取动作名, 应该有一份逻辑动作名对应表
		std::string skelName = *it;
		if ( !ExistResource( skelName ) )
		{
			*it = "";
		}
		else
		{
			Ogre::SkeletonPtr skelPtr = 
				Ogre::SkeletonManager::getSingleton().load(skelName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			Ogre::Animation* an = skelPtr->getAnimation(0);
			*it = an->getName();
		}

	}

	for ( uint i = 0; i < actionArr.size(); ++i )
	{
		mActionMap[descArr[i]] = actionArr[i];
		m_wndAnimCB.AddString(descArr[i].c_str());
	}
	


	mod->getMainNode()->setPosition(0,-80,0);
	
	m_wndAnimCheck.SetCheck(BST_CHECKED);


	// 设置角色的默认动画
	if ( !mActionMap.empty() )
	{
		std::string aniName = mActionMap.begin()->second;
		m_wndAnimCB.SetCurSel(0);	
		if ( !aniName.empty() )
			mod->setDefAnimation( aniName );
	}
}



void CAnimationDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CAnimationDlg::OnCbnSelchangeAnActionlist()
{	
	CString   sDesc;  
	GetDlgItemText(IDC_AN_ACTIONLIST,sDesc);
	std::string s = sDesc.GetBuffer();
	std::string sAction = mActionMap[s];

	if ( sAction == "" )
		return;

	bool check = (m_wndAnimCheck.GetCheck() != BST_UNCHECKED);
	

	if ( !g_Avatar )
		return;

	ModelGcs* mod = g_Avatar->getModel();
	if ( !mod )
		return;

	if ( check )
		mod->setDefAnimation(sAction);
	else
	{
		mod->setDefAnimation("");
		mod->playAnimation(sAction);
	}
}

void CAnimationDlg::OnBnClickedAnPlay()
{
	if ( !g_Avatar )
		return;

	ModelGcs* mod = g_Avatar->getModel();
	if ( mod )
	{
		mod->enableAnimation(true);
		if ( mod->isPauseAnimation() )
			mod->pauseAnimation(false);
		else
		{
			CString   str;  
			GetDlgItemText(IDC_AN_ACTIONLIST,str);
			if ( str != "" )
			{
				bool check = (m_wndAnimCheck.GetCheck() != BST_UNCHECKED);
				if ( check )
					mod->setDefAnimation(str.GetBuffer());
				else
				{
					mod->setDefAnimation("");
					mod->playAnimation(str.GetBuffer());
				}
			}
		}
	}
}

void CAnimationDlg::OnBnClickedAnPause()
{
	if ( g_Avatar )
		g_Avatar->getModel()->pauseAnimation( !g_Avatar->getModel()->isPauseAnimation() );
}

void CAnimationDlg::OnBnClickedAnStop()
{
	if ( g_Avatar )
		g_Avatar->getModel()->enableAnimation(false);
}


BOOL CAnimationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	resetContent();

	return TRUE;  
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAnimationDlg::OnBnClickedAnLoop()
{
	bool check = (m_wndAnimCheck.GetCheck() != BST_UNCHECKED);

	if ( !g_Avatar )
		return;

	CString   str;  
	GetDlgItemText(IDC_AN_ACTIONLIST,str);
	if ( !check )
	{
		g_Avatar->getModel()->setDefAnimation("");	
		g_Avatar->getModel()->playAnimation(str.GetBuffer());
	}
	else
		g_Avatar->getModel()->setDefAnimation( str.GetBuffer() );

}

void CAnimationDlg::OnBnClickedAnReset()
{

}

void CAnimationDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if ( (CSliderCtrl*)pScrollBar == &m_wndRateSlider )
	{
		int curr = m_wndRateSlider.GetPos();//取得当前位置值
		float rate = (float)curr/10.0f;

		std::string str = Ogre::StringConverter::toString(rate,2);
		str = "速率: " + str + "x";
		m_wndRateText.SetWindowText(str.c_str());

		if ( g_Avatar )
			g_Avatar->getModel()->modifyAnimationRate(rate);
	}
}

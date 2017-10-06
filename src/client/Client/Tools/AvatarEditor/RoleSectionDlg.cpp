// AvatarDlg.cpp : implementation file
//

#include "AvatarEditorPCH.h"
#include "AvatarTable.h"
#include "MyApp.h"
#include "RoleSectionDlg.h"
#include "Avatar.h"
#include "AvatarAdjuster.h"
#include "ItemTable.h"
#include "GraphicsTable.h"
#include "MainFrm.h"


BEGIN_MESSAGE_MAP(CRoleSectionDlg, CDialog)
	ON_BN_CLICKED(IDC_AV_UPPEROUTER, &CRoleSectionDlg::OnBnClickedAvUpperouter)
	ON_BN_CLICKED(IDC_AV_LOWERGARMENT, &CRoleSectionDlg::OnBnClickedAvLowergarment)
	ON_BN_CLICKED(IDC_AV_FOOT, &CRoleSectionDlg::OnBnClickedAvFoot)
	ON_BN_CLICKED(IDC_AV_HAT, &CRoleSectionDlg::OnBnClickedAvHat)
	ON_BN_CLICKED(IDC_AV_BACK, &CRoleSectionDlg::OnBnClickedAvBack)
	ON_BN_CLICKED(IDC_AV_ORNAMENT, &CRoleSectionDlg::OnBnClickedAvOrnament)
	ON_BN_CLICKED(IDC_AV_BAG, &CRoleSectionDlg::OnBnClickedAvBag)
	ON_BN_CLICKED(IDC_AV_LEFTHAND, &CRoleSectionDlg::OnBnClickedAvLefthand)
	ON_BN_CLICKED(IDC_AV_NOSE, &CRoleSectionDlg::OnBnClickedAvNose)
	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CRoleSectionDlg::OnDeltaposSpinHair)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, &CRoleSectionDlg::OnDeltaposSpinHead)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &CRoleSectionDlg::OnDeltaposSpinBeard)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, &CRoleSectionDlg::OnDeltaposSpinSkin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CRoleSectionDlg::OnDeltaposSpinHairColor)

	ON_BN_CLICKED(IDC_AV_SUIT, &CRoleSectionDlg::OnBnClickedAvSuit)
END_MESSAGE_MAP()

CRoleSectionDlg::CRoleSectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoleSectionDlg::IDD, pParent)
{
}

CRoleSectionDlg::~CRoleSectionDlg()
{
}

void CRoleSectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN1, mSpinButtonHair);
	DDX_Control(pDX, IDC_STATIC_1, mLabelOrnament);
	DDX_Control(pDX, IDC_STATIC_2, mLabelNose);
	DDX_Control(pDX, IDC_STATIC_3, mLabelHand);
	DDX_Control(pDX, IDC_STATIC_4, mLabelHat);
	DDX_Control(pDX, IDC_STATIC_5, mLabelBack);
	DDX_Control(pDX, IDC_STATIC_6, mLabelShoes);
	DDX_Control(pDX, IDC_STATIC_7, mLabelPants);
	DDX_Control(pDX, IDC_STATIC_8, mLabelCoat);
	DDX_Control(pDX, IDC_STATIC_13, mLabelBeard);
	DDX_Control(pDX, IDC_STATIC_12, mLabelSkin);
	DDX_Control(pDX, IDC_STATIC_11, mLabelHead);
	DDX_Control(pDX, IDC_STATIC_10, mLabelHairColor);
	DDX_Control(pDX, IDC_STATIC_9, mLabelHair);
	DDX_Control(pDX, IDC_SPIN2, mSpinButtonHairColor);
	DDX_Control(pDX, IDC_SPIN3, mSpinButtonHead);
	DDX_Control(pDX, IDC_SPIN4, mSpinButtonSkin);
	DDX_Control(pDX, IDC_SPIN5, mSpinButtonBeard);
	DDX_Control(pDX, IDC_STATIC_14, mLabelSuit);
}



void CRoleSectionDlg::OnCancel()
{
	//CDialog::OnCancel();
}

BOOL CRoleSectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_wndItemDlg.Create(CAvatarItemDlg::IDD, NULL);
	m_wndItemDlg.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control	
}


void CRoleSectionDlg::OnBnClickedAvSuit()
{
	m_wndItemDlg.showItems( IEBT_SUIT );
}

void CRoleSectionDlg::OnBnClickedAvUpperouter()
{	
	m_wndItemDlg.showItems( IEBT_COAT );
}

void CRoleSectionDlg::OnBnClickedAvLowergarment()
{
	m_wndItemDlg.showItems( IEBT_PANTS );
}

void CRoleSectionDlg::OnBnClickedAvFoot()
{
	m_wndItemDlg.showItems( IEBT_SHOES );
}

void CRoleSectionDlg::OnBnClickedAvHat()
{
	m_wndItemDlg.showItems( IEBT_HAT );
}

void CRoleSectionDlg::OnBnClickedAvBack()
{
	m_wndItemDlg.showItems( IEBT_BACK );
}

void CRoleSectionDlg::OnBnClickedAvOrnament()
{
	m_wndItemDlg.showItems( IEBT_ORNAMENT );
}

void CRoleSectionDlg::OnBnClickedAvBag()
{
	m_wndItemDlg.showItems( IEBT_BACK );
}

void CRoleSectionDlg::OnBnClickedAvLefthand()
{
	m_wndItemDlg.showItems( IEBT_HAND );
}

void CRoleSectionDlg::OnBnClickedAvNose()
{
	m_wndItemDlg.showItems( IEBT_NOSE );
}


void CRoleSectionDlg::OnDeltaposSpinHair(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	if ( !g_Avatar )
		return;

	if ( pNMUpDown->iDelta > 0 ) 
		g_Avatar->getAdjuster()->prevHairType();
	else
		g_Avatar->getAdjuster()->nextHairType();


	mLabelHair.SetWindowTextA( g_Avatar->getAdjuster()->getHairMesh().c_str() );
}

void CRoleSectionDlg::OnDeltaposSpinHead(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	if ( !g_Avatar )
		return;
	if ( pNMUpDown->iDelta > 0 ) 
		g_Avatar->getAdjuster()->prevFace();
	else
		g_Avatar->getAdjuster()->nextFace();
	
	mLabelHead.SetWindowTextA( g_Avatar->getAdjuster()->getHeadMesh().c_str() );
}


void CRoleSectionDlg::OnDeltaposSpinBeard(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	if ( !g_Avatar )
		return;

	if ( pNMUpDown->iDelta > 0 ) 
		g_Avatar->getAdjuster()->prevBeard();
	else
		g_Avatar->getAdjuster()->nextBeard();

	mLabelBeard.SetWindowTextA( g_Avatar->getAdjuster()->getBeardMesh().c_str() );
}


void CRoleSectionDlg::OnDeltaposSpinSkin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
	if ( !g_Avatar )
		return;

	if ( pNMUpDown->iDelta > 0 ) 
		g_Avatar->getAdjuster()->prevSkin();
	else
		g_Avatar->getAdjuster()->nextSkin();

	this->mLabelSkin.SetWindowTextA( Ogre::StringConverter::toString( g_Avatar->getAdjuster()->getSkinColorID() ).c_str() );
	this->mLabelHead.SetWindowTextA( g_Avatar->getAdjuster()->getHeadMesh().c_str() );
	//this->mLabelBody.SetWindowTextA( g_Avatar->getAdjuster()->getBodyMesh().c_str() );
}

void CRoleSectionDlg::OnDeltaposSpinHairColor(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	*pResult = 0;
	if ( !g_Avatar )
		return;

	if ( pNMUpDown->iDelta > 0 ) 
		g_Avatar->getAdjuster()->prevHairColor();
	else
		g_Avatar->getAdjuster()->nextHairColor();

	this->mLabelHairColor.SetWindowTextA( Ogre::StringConverter::toString( g_Avatar->getAdjuster()->getHairColorID() ).c_str() );
	this->mLabelHair.SetWindowTextA( g_Avatar->getAdjuster()->getHairMesh().c_str() );
	this->mLabelBeard.SetWindowTextA( g_Avatar->getAdjuster()->getBeardMesh().c_str() );
}


void CRoleSectionDlg::resetContent()
{
	this->mLabelBack.SetWindowTextA( "" );
	this->mLabelBeard.SetWindowTextA( "" );
	this->mLabelCoat.SetWindowTextA( "" );
	this->mLabelHair.SetWindowTextA( "" );
	this->mLabelHairColor.SetWindowTextA( "" );
	this->mLabelHand.SetWindowTextA( "" );
	this->mLabelHat.SetWindowTextA( "" );
	this->mLabelHead.SetWindowTextA( "" );
	this->mLabelNose.SetWindowTextA( "" );
	this->mLabelOrnament.SetWindowTextA( "" );
	this->mLabelPants.SetWindowTextA( "" );
	this->mLabelShoes.SetWindowTextA( "" );
	this->mLabelSkin.SetWindowTextA( "" );
	this->mLabelSuit.SetWindowTextA( "" );

	// 清空选项
	this->m_wndItemDlg.resetContent();
}


void CRoleSectionDlg::setLabelText( ItemEquipBigType bigType, const std::string & text )
{
	switch ( bigType )
	{
	case IEBT_HAT:
		mLabelHat.SetWindowTextA( text.c_str() );
		break;
	case IEBT_COAT:
		mLabelCoat.SetWindowTextA( text.c_str() );
		break;
	case IEBT_PANTS:
		mLabelPants.SetWindowTextA( text.c_str() );
		break;
	case IEBT_SHOES:
		mLabelShoes.SetWindowTextA( text.c_str() );
		break;
	case IEBT_NOSE:
		mLabelNose.SetWindowTextA( text.c_str() );
		break;
	case IEBT_BACK:
		mLabelBack.SetWindowTextA( text.c_str() );
		break;
	case IEBT_HAND:
		mLabelHand.SetWindowTextA( text.c_str() );
		break;
	case IEBT_ORNAMENT:
		mLabelOrnament.SetWindowTextA( text.c_str() );
		break;
	case IEBT_SUIT:
		mLabelSuit.SetWindowTextA( text.c_str() );
		break;
	default:
		break;
	}			
}

void CRoleSectionDlg::notifyAvatarCreate()
{
	// 更新物品
	m_wndItemDlg.reloadItems();
	
	this->mLabelSkin.SetWindowTextA( Ogre::StringConverter::toString( g_Avatar->getAdjuster()->getSkinColorID() ).c_str() );
	this->mLabelHead.SetWindowTextA( g_Avatar->getAdjuster()->getHeadMesh().c_str() );
	//this->mLabelBody.SetWindowTextA( g_Avatar->getAdjuster()->getBodyMesh().c_str() );
	this->mLabelHairColor.SetWindowTextA( Ogre::StringConverter::toString( g_Avatar->getAdjuster()->getHairColorID() ).c_str() );
	this->mLabelHair.SetWindowTextA( g_Avatar->getAdjuster()->getHairMesh().c_str() );
	this->mLabelBeard.SetWindowTextA( g_Avatar->getAdjuster()->getBeardMesh().c_str() );
}


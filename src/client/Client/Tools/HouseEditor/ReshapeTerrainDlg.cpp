// ReshapeTerrainDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "ReshapeTerrainDlg.h"
#include "TerrainData.h"
#include "SceneManager.h"
#include "SceneInfo.h"
#include "RootManager.h"


// CReshapeTerrainDlg dialog

IMPLEMENT_DYNAMIC(CReshapeTerrainDlg, CDialog)

CReshapeTerrainDlg::CReshapeTerrainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReshapeTerrainDlg::IDD, pParent)
{
	mType = 0;
}

CReshapeTerrainDlg::~CReshapeTerrainDlg()
{
}

void CReshapeTerrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RT_LEFT, mLeftCtl);
	DDX_Control(pDX, IDC_RT_RIGHT, mRightCtl);
	DDX_Control(pDX, IDC_RT_TOP, mTopCtl);
	DDX_Control(pDX, IDC_RT_DOWN, mDownCtl);
}



BOOL CReshapeTerrainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( mType == 1 )
		SetWindowText("扩容地图边界");


	mLeftCtl.SetWindowText("0");
	mRightCtl.SetWindowText("0");
	mTopCtl.SetWindowText("0");
	mDownCtl.SetWindowText("0");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_MESSAGE_MAP(CReshapeTerrainDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CReshapeTerrainDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CReshapeTerrainDlg::OnBnClickedOk()
{
	CString strLeft;
	CString strRight;
	CString strTop;
	CString strDown;

	mLeftCtl.GetWindowText(strLeft);
	mRightCtl.GetWindowText(strRight);
	mTopCtl.GetWindowText(strTop);
	mDownCtl.GetWindowText(strDown);

	mLeft = Ogre::StringConverter::parseInt(strLeft.GetBuffer());
	mRight = Ogre::StringConverter::parseInt(strRight.GetBuffer());
	mTop = Ogre::StringConverter::parseInt(strTop.GetBuffer());
	mDown = Ogre::StringConverter::parseInt(strDown.GetBuffer());

	if ( mLeft < -100 || mLeft > 100
		|| mRight < -100 || mRight > 100
		|| mTop < -100 || mTop > 100
		|| mDown < -100 || mDown > 100
		)
	{
		MessageBox("扩容的范围(-100~100)!","输入错误");
		return;
	}

	if ( mLeft == 0 && mRight == 0 && mTop == 0 && mDown == 0 )
	{
		MessageBox("好歹填个非0值吧!", "提示");
		return;
	}

	if ( mType == 0 )
	{
		TerrainData* terrainData = EQSceneManager::getSingleton().getTerrainData();
		assert (terrainData);

		int xSize = terrainData->getXSize();
		int zSize = terrainData->getZSize();

		mRLeft = -mLeft;
		mRRight = xSize + mRight;
		mRTop = -mTop;
		mRDown = zSize + mDown;

		if ((mRLeft >= mRRight) || (mRTop >= mRDown))
		{
			MessageBox("输入值不合理!", "输入错误");
			return;
		}
	}
	else if (mType == 1)
	{
		SceneInfo* info = EQSceneManager::getSingleton().getSceneInfo();
		Ogre::Vector2 b = info->mSceneBeginPos;
		Ogre::Vector2 e = info->mSceneEndPos;

		b.x -= mLeft*80.0f;
		b.y -= mTop*80.0f;
		e.x += mRight*80.0f;
		e.y += mDown*80.0f;

		if ( b.x >= e.x || b.y >= e.y )
		{
			MessageBox("输入值不合理!", "输入错误");
			return;
		}

		info->mSceneBeginPos = b;
		info->mSceneEndPos = e;

		RootManager::getSingleton().reshapeTerrainEdge();
	}


	//
	OnOK();
}

void CReshapeTerrainDlg::setTypeModifyEdge()
{

	mType = 1;
}

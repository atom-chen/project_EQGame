// PathSearchDlg.cpp : implementation file
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "PathSearchDlg.h"
#include "RootManager.h"
#include "RegionEditAction.h"
#include "MfcInputEventMgr.h"
#include "ReshapeTerrainDlg.h"
#include "SceneManager.h"
#include "SceneInfo.h"
#include "SystemGcs.h"
#include "TerrainData.h"
#include "CommFuncGcs.h"
#include "UINavMeshEvent.h"

CPathSearchDlg* g_CPathSearchDlg = NULL;

// CPathSearchDlg dialog

IMPLEMENT_DYNAMIC(CPathSearchDlg, CDialog)

CPathSearchDlg::CPathSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPathSearchDlg::IDD, pParent)
	, mRem(NULL)
	,m_bIsEditingRegion(false)
	//,m_iCurSelRegionIndex(-1)
	, mNavMeshLine(NULL)
	, mNss(0.01f)
{
	g_CPathSearchDlg = this;

	m_ulCurRegionId = 0;	
//	m_ulCurPointId  = 0;	
	m_ulCurLineId   = 0;	

	m_bIsChangeCamera = true;
}

CPathSearchDlg::~CPathSearchDlg()
{
	if ( mNavMeshLine )
	{
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mNavMeshLine->getName());
		mNavMeshLine = NULL;
	}

	mNss.destroy();

	if ( mRem )
		mRem->ClearRegionData();
}

void CPathSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PS_REGION_LIST, mRegionListCtl);
	DDX_Control(pDX, IDC_PS_POINT_LIST, mPointListCtl);
}


BEGIN_MESSAGE_MAP(CPathSearchDlg, CDialog)
	ON_BN_CLICKED(IDC_PS_ADDREGION, &CPathSearchDlg::OnBnClickedPsAddregion)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_PS_DELREGION, &CPathSearchDlg::OnBnClickedPsDelregion)
	ON_LBN_SELCHANGE(IDC_PS_REGION_LIST, &CPathSearchDlg::OnLbnSelchangePsRegionList)
	ON_LBN_DBLCLK(IDC_PS_REGION_LIST, &CPathSearchDlg::OnLbnDblclkPsRegionList)
	ON_BN_CLICKED(IDC_PS_CREATEAREA, &CPathSearchDlg::OnBnClickedPsCreatearea)
	ON_LBN_SELCHANGE(IDC_PS_POINT_LIST, &CPathSearchDlg::OnLbnSelchangePsPointList)
	ON_BN_CLICKED(IDC_BUTTON4, &CPathSearchDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_PS_CHECKCREATEALLAREA, &CPathSearchDlg::OnBnClickedPsCheckcreateallarea)
	ON_BN_CLICKED(IDC_PS_REFLASHEDGE, &CPathSearchDlg::OnBnClickedPsReflashedge)
	ON_BN_CLICKED(IDC_PS_MODIFYEDGE, &CPathSearchDlg::OnBnClickedPsModifyedge)
	ON_BN_CLICKED(IDC_PS_CREATE_NAVGRID, &CPathSearchDlg::OnBnClickedPsCreateNavgrid)
	ON_BN_CLICKED(IDC_PS_SHOW_NAV, &CPathSearchDlg::OnBnClickedPsShowNav)
	ON_BN_CLICKED(IDC_PS_TESTNAVPATH, &CPathSearchDlg::OnBnClickedPsTestnavpath)
END_MESSAGE_MAP()


// CPathSearchDlg message handlers

void CPathSearchDlg::notifySceneDestroy()
{
	if ( mNavMeshLine )
	{
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mNavMeshLine->getName());
		mNavMeshLine = NULL;
	}

	mNss.destroy();

	clearRegion();
}

void CPathSearchDlg::clearRegion()
{
	//这个如果是在析构函数时候调用, 会导致线程异常退出, 非常恶心
	mRegionListCtl.ResetContent();
	mPointListCtl.ResetContent();

	if ( mRem )
		mRem->ClearRegionData();

}

void CPathSearchDlg::refreshRegion()
{
	mRegionListCtl.ResetContent();
	mPointListCtl.ResetContent();

	// 
	UNREACHREGIONMAP::iterator itRegion;
	UNREACHREGIONMAP::iterator itEndRegion;
	unsigned long   ulId    = 0;
	int				iCount  = 0;

	CUnreachRegion* pRegion = NULL;


	mRem = RootManager::getSingleton().getRegionEditManager();
	char buf[256];
	// 存储每个区域的信息
	itEndRegion  = mRem->m_UnreachRegionMap.end();
	for(itRegion = mRem->m_UnreachRegionMap.begin(); itRegion != itEndRegion; itRegion++)
	{	
		pRegion = itRegion->second;
		if(pRegion)
		{
			memset(buf, 0, sizeof(buf));

			// 存储区域的id
			ulId = pRegion->m_ulId;
			sprintf(buf, "区域%03d", ulId);

			iCount  = mRegionListCtl.AddString(buf);
			mRegionListCtl.SetItemData(iCount, ulId);
		}
	}

	if(mRegionListCtl.GetCount()>0)
	{
		mRegionListCtl.SetSel(0);
	}

}

void CPathSearchDlg::loadRegion(const std::string& filename)
{
	clearRegion();

	if (0 == RootManager::getSingleton().getRegionEditManager()->ReadRegionFromFile(filename) )
		refreshRegion();
}

void CPathSearchDlg::saveNav(const std::string& filename)
{
	mNss.save(filename);
}

void CPathSearchDlg::loadNav(const std::string& filename)
{
	if ( mNavMeshLine )
	{
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mNavMeshLine->getName());
		mNavMeshLine = NULL;
	}

	if ( mNss.load(filename) )
		updateNavMeshLine();
}

void CPathSearchDlg::OnBnClickedPsAddregion()
{
	AddNewRegion();
}

void CPathSearchDlg::SetActive(bool bShow)
{
	if ( !mRem )
		mRem = RootManager::getSingleton().getRegionEditManager();

	mRem->HideScene(bShow);
	if (bShow)
		mRem->SetRegionHeight();


	if ( bShow )
	{
		Action* oa = MfcInputEventMgr::getSingleton().getEvent<Action>();
		if ( oa && oa->getName() != "CRegionEditAction" )
			RootManager::getSingleton().setActiveAction("CRegionEditAction");
		else if ( oa == NULL )
			RootManager::getSingleton().setActiveAction("CRegionEditAction");
	}
	else
		RootManager::getSingleton().clearActiveAction();
}

void CPathSearchDlg::AddNewRegion()
{
	SetActive(true);

	// 添加一个新的区域
	char buf[256];

	mRem->UnSelRegion(m_ulCurRegionId);
	m_ulCurRegionId = mRem->AddNewRegion();

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "区域%03d", m_ulCurRegionId);

	int ix  = mRegionListCtl.AddString(buf);
	mRegionListCtl.SetItemData(ix, m_ulCurRegionId);
	mRegionListCtl.SetCurSel(ix);

	mPointListCtl.ResetContent();
}

void CPathSearchDlg::DelRegion()
{
	// 删除当前区域后需要选中的区域的index
	int iNextIndex   = -1;
	int iIndex       = mRegionListCtl.GetCurSel();
	int iRegionCount = mRegionListCtl.GetCount();

	unsigned long ulRegionId = 0;
	// 删除当前区域后需要选中的区域的ID
	unsigned long ulNextRegionId = 0;

	if(-1 != iIndex)
	{
		if(iIndex > 0)
		{
			// 如果不是选择删除的头一个区域
			// 下一个选择的区域就是当前选择的前一个区域
			iNextIndex = iIndex-1;
		}
		else
		{
			if(iRegionCount > 1)
			{

				iNextIndex = iIndex+1;

			}
			else
			{
				iNextIndex = -1;
			}
		}

		if(-1 != iNextIndex)
		{
			ulNextRegionId = (unsigned long)mRegionListCtl.GetItemData(iNextIndex);
		}

		// 得到区域id
		ulRegionId = (unsigned long)mRegionListCtl.GetItemData(iIndex);
		//if(m_pRegionEditAction)
		{
			// 删除当前的region数据
			mRem->DelRegion(ulRegionId);



			//设置当前正在编辑的区域
			m_ulCurRegionId = ulNextRegionId;
			mRem->SelRegion(ulNextRegionId, false);

			// 清空以前的数据， 填充点的数据
			mPointListCtl.ResetContent();


			// 得到下一个要选择的区域
			CUnreachRegion* pRegion = mRem->GetRegionById(ulNextRegionId);
			if(pRegion)
			{
				int iPointCount = pRegion->GetPointCount();
				CPointInfo* pPointInfo = NULL;
				for(int i = 0; i < iPointCount; i++)
				{
					pPointInfo = pRegion->GetPointInfoByIndex(i);
					if(pPointInfo)
					{
						RegionAddNewPoint(pPointInfo->m_ulPointId, true);
					}
				}//
			}

		}//if(m_pRegionEditAction)

		if(-1 != iNextIndex)
		{
			mRegionListCtl.SetCurSel(iNextIndex);
		}

		mRegionListCtl.DeleteString(iIndex);

		// 设置当前没有编辑区域标记
		//SetEditRegionFlag(false);


	}// if(-1 != iIndex)

}

// 在当前的区域添加一个新的点
void CPathSearchDlg::RegionAddNewPoint(unsigned long ulId, bool bRefresh)
{
	if(0 == ulId)
	{
		// 当前点的id不合法
		return;
	}

	if(0 == m_ulCurRegionId)
	{
		// 当前区域的id不合法
		return;
	}

	// 添加一个新的区域
	char buf[256];
	//if(m_pRegionEditAction)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "区域%03d->点%02d", m_ulCurRegionId, ulId);

		int iCount = mPointListCtl.GetCount();
		int ix = mPointListCtl.AddString(buf);
		mPointListCtl.SetItemData(ix, ulId);

		// 当前编辑的点
		//m_ulCurPointId = ulId;

		mPointListCtl.SetCurSel(ix);

		if(!bRefresh)
		{
			// 设置当前正在编辑区域
			//SetEditRegionFlag(true);
		}

	}
}

void CPathSearchDlg::DoSelRegion()
{
	int iIndex = mRegionListCtl.GetCurSel();

	// 记录当前在listbox中选择的区域index
	unsigned long ulRegionId = 0;
	if(-1 != iIndex)
	{

		// 得到区域id
		ulRegionId = (unsigned long)mRegionListCtl.GetItemData(iIndex);
		//if(m_pRegionEditAction)
		{
			CUnreachRegion* pRegion = mRem->GetRegionById(ulRegionId);
			if(NULL == pRegion)
			{
				return ;
			}

			mRem->UnSelRegion(m_ulCurRegionId);
			// 当前正在编辑的区域
			m_ulCurRegionId = ulRegionId;
			mRem->SelRegion(ulRegionId, false);

			// 清空以前的数据.
			mPointListCtl.ResetContent();

			int iPointCount = pRegion->GetPointCount();
			CPointInfo* pPointInfo = NULL;
			for(int i = 0; i < iPointCount; i++)
			{
				pPointInfo = pRegion->GetPointInfoByIndex(i);
				if(pPointInfo)
				{
					RegionAddNewPoint(pPointInfo->m_ulPointId, true);
				}

			}//


		}
	}
}

void CPathSearchDlg::UpdateCamera()
{
	int iIndex = mRegionListCtl.GetCurSel();

	// 记录当前在listbox中选择的区域index
	unsigned long ulRegionId = 0;
	if(-1 != iIndex)
	{
		// 得到区域id
		ulRegionId = (unsigned long)mRegionListCtl.GetItemData(iIndex);
		CUnreachRegion* pRegion = mRem->GetRegionById(ulRegionId);
		if(NULL == pRegion)
			return;

		Ogre::Vector3 point(Ogre::Vector3::ZERO);
		for ( size_t i = 0; i < pRegion->m_pLineShape->m_pointArray.size(); ++i)
			point += pRegion->m_pLineShape->m_pointArray[i];
		
		if ( pRegion->m_pLineShape->m_pointArray.empty() )
			return;
		point /= pRegion->m_pLineShape->m_pointArray.size();

		mRem->SetCamera(point.x, point.z);
	}
}

bool CPathSearchDlg::FinishRegion(unsigned long id)
{
	if(!mRem->IsRegionValidate(id))
	{
		std::string str = "区域" + Ogre::StringConverter::toString(id) + "含有不合法的点， 请改正。";
		MessageBox(str.c_str(), _T("编辑区域！"));
		return false;
	}

	mRem->ReshapeArea(id);
	mRem->HideRegionArea(id, false);

	return true;
}

bool CPathSearchDlg::CheckAndCreateAllRegion()
{
	UNREACHREGIONMAP& r = *mRem->GetUnreachRegionMap();
	for(UNREACHREGIONMAP::iterator it = r.begin(); it != r.end(); ++it)
	{
		if ( !FinishRegion(it->first) )
			return false;
	}
	return true;
}

void CPathSearchDlg::DoSelPoint()
{

	// 得到选中的索引
	int iIndex = mPointListCtl.GetCurSel();

	unsigned long ulPointId = 0;
	if(iIndex>=0)
	{
		// 得到区域id
		ulPointId = (unsigned long)mPointListCtl.GetItemData(iIndex);
		mRem->SelPoint(ulPointId);
	}
}

void CPathSearchDlg::RefreshRegionPointData(unsigned long ulRegionId)
{
	CUnreachRegion* pRegion = mRem->GetRegionById(ulRegionId);
	if(NULL == pRegion)
	{
		return ;
	}

	// 清空以前的数据.
	mPointListCtl.ResetContent();

	int iPointCount = pRegion->GetPointCount();
	CPointInfo* pPointInfo = NULL;
	for(int i = 0; i < iPointCount; i++)
	{
		pPointInfo = pRegion->GetPointInfoByIndex(i);
		if(pPointInfo)
		{
			RegionAddNewPoint(pPointInfo->m_ulPointId, true);
		}
	}//
}

void CPathSearchDlg::DelRegionPoint(unsigned long ulPointId)
{
	int iCount = mPointListCtl.GetCount();
	if ( iCount > 0)
		mPointListCtl.DeleteString(iCount - 1);

	if(iCount > 1)
	{
		// 得到选中的索引
		int iIndex = iCount - 2;

		mPointListCtl.SetCurSel(iIndex);
		unsigned long ulPointId = 0;
		if(-1 != iIndex)
		{
			// 得到区域id
			ulPointId = (unsigned long)mPointListCtl.GetItemData(iIndex);
			mRem->SelPoint(ulPointId);
		}
	}
}

void CPathSearchDlg::onRegionAddNewPoint(unsigned long ulRegionId,unsigned long ulPointId, Ogre::Vector3 position)
{
	// 只有当前区域是这个区域时才添加.
	if(m_ulCurRegionId == ulRegionId)
	{
		RegionAddNewPoint(ulPointId);
	}
}

void CPathSearchDlg::onRegionDelPoint(unsigned long ulRegionId,unsigned long ulPointId)
{
	// 只有当前区域是这个区域时才添加.
	if(m_ulCurRegionId == ulRegionId)
	{
		DelRegionPoint(ulPointId);
	}
}

// 选中一个区域的回调函数。
void CPathSearchDlg::onRegionSel(unsigned long ulRegionId)
{
	unsigned long ulFindRegionId = 0;

	int iRegionCount = mRegionListCtl.GetCount();

	for(int i = 0; i < iRegionCount; i++)
	{
		// 得到区域id
		ulFindRegionId = (unsigned long)mRegionListCtl.GetItemData(i);
		if(ulFindRegionId == ulRegionId)
		{
			m_bIsChangeCamera = false;
			mRegionListCtl.SetCurSel(i);
			DoSelRegion();

			m_bIsChangeCamera = true;
			break;
		}

	}
}

void CPathSearchDlg::OnOK()
{
}

void CPathSearchDlg::OnCancel()
{
}

void CPathSearchDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if ( !bShow )
	{
		RootManager::getSingleton().clearActiveAction();
		MfcInputEventMgr::getSingleton().delEvent<TestNavMeshPathEvent>();
	}

	mRem = RootManager::getSingleton().getRegionEditManager();
	if ( mRem )
	{
		if ( bShow )
		{
			mRem->setListener(this);
			SetActive(true);
		}
		else
		{
			mRem->setListener(NULL);
			SetActive(false);
			getNavMeshLineNode()->setVisible(false);
		}
	}
}

void CPathSearchDlg::OnBnClickedPsDelregion()
{
	if ( IDYES != MessageBox("你确定要删除这个区域吗？", "删除区域提示", MB_YESNO) )
		return;

	DelRegion();
}

void CPathSearchDlg::OnLbnSelchangePsRegionList()
{
	DoSelRegion();
}

void CPathSearchDlg::OnLbnDblclkPsRegionList()
{
	UpdateCamera();

	Action* oa = MfcInputEventMgr::getSingleton().getEvent<Action>();
	if ( oa && oa->getName() != "CRegionEditAction" || oa == NULL )
		RootManager::getSingleton().setActiveAction("CRegionEditAction");
}

void CPathSearchDlg::OnBnClickedPsCreatearea()
{
	int ix = mRegionListCtl.GetCurSel();
	if ( ix < 0 )
		return;
	unsigned long id = (unsigned long)mRegionListCtl.GetItemData(ix);
	FinishRegion(id);
}

void CPathSearchDlg::OnLbnSelchangePsPointList()
{
	DoSelPoint();
}

void CPathSearchDlg::OnBnClickedButton4()
{
	MessageBox("双击区域列表可以快速定位区域位置\nCtrl+左击视图中的选区可以选择区域\nShift+左击拖拽可以调整点的位置", "帮助");
}

void CPathSearchDlg::OnBnClickedPsCheckcreateallarea()
{
	if ( CheckAndCreateAllRegion() )
		MessageBox("执行成功!","提示");
}

void CPathSearchDlg::OnBnClickedPsReflashedge()
{
	RootManager::getSingleton().reshapeTerrainEdge();
}

void CPathSearchDlg::OnBnClickedPsModifyedge()
{
	CReshapeTerrainDlg dlg;
	dlg.setTypeModifyEdge();
	if( dlg.DoModal() != IDOK )
		return;
}

void CPathSearchDlg::OnBnClickedPsCreateNavgrid()
{
	if ( !CheckAndCreateAllRegion() )
		return;

	if ( mNavMeshLine )
	{
		SystemGcs::getSingleton().getSceneManager()->destroyManualObject(mNavMeshLine->getName());
		mNavMeshLine = NULL;
	}

	std::vector<SearchSystem::Polygon> ploys;

	//增加外边界
	Ogre::Vector2 bp = EQSceneManager::getSingleton().getSceneInfo()->mSceneBeginPos;
	Ogre::Vector2 ep = EQSceneManager::getSingleton().getSceneInfo()->mSceneEndPos;
	//SearchSystem::Polygon py;
	//py.addVertex(SearchSystem::Vector2f(bp.x, bp.y));
	//py.addVertex(SearchSystem::Vector2f(bp.x, ep.y));
	//py.addVertex(SearchSystem::Vector2f(ep.x, ep.y));
	//py.addVertex(SearchSystem::Vector2f(ep.x, bp.y));
	//ploys.push_back(py);

	UNREACHREGIONMAP& r = *mRem->GetUnreachRegionMap();
	for(UNREACHREGIONMAP::iterator it = r.begin(); it != r.end(); ++it)
	{
		CUnreachRegion* ur = it->second;

		if ( ur->m_pointMap.size() < 3 )
			continue;

		SearchSystem::Polygon py;
		for ( POINT_MAP::iterator it = ur->m_pointMap.begin(); it != ur->m_pointMap.end(); ++it )
		{
			CPointInfo* pt = it->second;
			py.addVertex(SearchSystem::Vector2f(pt->m_Position.x, pt->m_Position.z));
		}
		ploys.push_back(py);
	}

	//
	mNss.setEdge(SearchSystem::Vector2f(bp.x, bp.y), SearchSystem::Vector2f(ep.x, ep.y));
	mNss.build(ploys);

	//
	updateNavMeshLine();

}

Ogre::SceneNode*  CPathSearchDlg::getNavMeshLineNode()
{
	std::string nameNode = "--NavMeshLineNode--";

	Ogre::SceneNode* node;
	if ( SystemGcs::getSingleton().getSceneManager()->hasSceneNode(nameNode) )
		node = SystemGcs::getSingleton().getSceneManager()->getSceneNode(nameNode);
	else
		node = SystemGcs::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode(nameNode);

	return node;
}

void CPathSearchDlg::updateNavMeshLine()
{
	Ogre::SceneNode* node = getNavMeshLineNode();


	TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();
	std::vector<SearchSystem::Cell>& cells = mNss.getCells();
	if ( cells.empty() )
		return;

	std::string name = "--LNavMeshLine--";
	std::string matName = "SystemMat/Comm-2";

	Ogre::ManualObject* manual = SystemGcs::getSingleton().getSceneManager()->createManualObject(name);
	manual->begin(matName, Ogre::RenderOperation::OT_LINE_LIST);
	for (size_t i = 0; i < cells.size(); ++i)
	{
		SearchSystem::Vector2f a = cells[i].getPointA();
		SearchSystem::Vector2f b = cells[i].getPointB();
		SearchSystem::Vector2f c = cells[i].getPointC();
		Ogre::Vector3 aa(a.x, terr->getHeightAt(a.x, a.y) + 5, a.y);
		Ogre::Vector3 bb(b.x, terr->getHeightAt(b.x, b.y) + 5, b.y);
		Ogre::Vector3 cc(c.x, terr->getHeightAt(c.x, c.y) + 5, c.y);

		Ogre::ColourValue clr;

		clr = cells[i].getLink(0) == -1 ? Ogre::ColourValue(1,0,0) : Ogre::ColourValue(1,1,0);
		manual->position(aa);
		manual->colour(clr);
		manual->position(bb);
		manual->colour(clr);

		clr = cells[i].getLink(1) == -1 ? Ogre::ColourValue(1,0,0) : Ogre::ColourValue(1,1,0);
		manual->position(bb);
		manual->colour(clr);
		manual->position(cc);
		manual->colour(clr);

		clr = cells[i].getLink(2) == -1 ? Ogre::ColourValue(1,0,0) : Ogre::ColourValue(1,1,0);
		manual->position(cc);
		manual->colour(clr);
		manual->position(aa);
		manual->colour(clr);
	}
	manual->end();

	node->attachObject(manual);

	mNavMeshLine = manual;
}


void CPathSearchDlg::OnBnClickedPsShowNav()
{
	if ( !mNavMeshLine )
		return;

	Ogre::SceneNode* node = getNavMeshLineNode();
	node->setVisible(!mNavMeshLine->getVisible());
}

void CPathSearchDlg::OnBnClickedPsTestnavpath()
{
	MfcInputEventMgr::getSingleton().addEvent<TestNavMeshPathEvent>();
}

#pragma once
#include "afxwin.h"

#include "RegionEditAction.h"
#include "SearchSystem/NavSearchSystem.h"


// CPathSearchDlg dialog
class CPathSearchDlg : public CDialog , public RegionEditManager::Listener
{
	DECLARE_DYNAMIC(CPathSearchDlg)

public:
	CPathSearchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPathSearchDlg();

// Dialog Data
	enum { IDD = IDD_PATH_SEARCH };

	void notifySceneDestroy();

	void clearRegion();

	void refreshRegion();

	void loadRegion(const std::string& filename);

	void saveNav(const std::string& filename);

	void loadNav(const std::string& filename);

public:
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// 不可行走区域数据.
	//
	unsigned long m_ulCurRegionId;		// 当前编辑的区域id.
	//unsigned long m_ulCurPointId;		// 当前编辑的点的id.
	unsigned long m_ulCurLineId;		// 当前编辑的线的id.

	bool		  m_bIsChangeCamera;	    // 是否改变摄像机位置。
	bool		  m_bIsEditingRegion;		// 是否正在编辑区域。

	//int			  m_iCurSelRegionIndex;

public:
	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// 场景操作
	//

	// 激活区域编辑操作
	void SetActive(bool bShow);

	///////////////////////////////////////////////////////////////////////////////////////////
	//
	// 逻辑数据操作
	//

	// 添加一个区域.
	void AddNewRegion();

	// 删除一个区域
	void DelRegion();

	// 在当前的区域添加一个新的点
	void RegionAddNewPoint(unsigned long ulId, bool bRefresh = false);

	// 处理当前选中的区域
	void DoSelRegion();

	// 更新摄像机
	void UpdateCamera();

	// 完成一个区域的编辑
	bool FinishRegion(unsigned long id);

	// 检查并且创建所有选区
	bool CheckAndCreateAllRegion();

	///////////////////////////////////////////////////////////////////////////////////////////
	//
	// 点的逻辑操作
	//

	// 选中点的操作
	void DoSelPoint();

	// 把点的信息刷新到界面上
	void RefreshRegionPointData(unsigned long ulRegionId);

	// 删除区域中的一个点
	void DelRegionPoint(unsigned long ulPointId);



	//////////////////////////////////////////////////////////////////////////////////////////
	//
	// 界面回掉函数操作
	//

	//不可行走区域回掉函数
	virtual void onRegionAddNewPoint(unsigned long ulRegionId,unsigned long ulPointId, Ogre::Vector3 position);

	// 删除一个点的回调函数
	virtual void onRegionDelPoint(unsigned long ulRegionId,unsigned long ulPointId);

	// 选中一个区域
	virtual void onRegionSel(unsigned long ulRegionId);

	//////////////////////////////////////////////////////////////////////////////////////////
	//
	// 导航网格相关
	//
	Ogre::ManualObject*	mNavMeshLine;

	SearchSystem::NavSearchSystem mNss;

	//获取导航网格节点
	Ogre::SceneNode* getNavMeshLineNode();

	//更新导航网格线
	void updateNavMeshLine();

protected:
	RegionEditManager* mRem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPsAddregion();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CListBox mRegionListCtl;
	CListBox mPointListCtl;
	afx_msg void OnBnClickedPsDelregion();
	afx_msg void OnLbnSelchangePsRegionList();
	afx_msg void OnLbnDblclkPsRegionList();
	afx_msg void OnBnClickedPsCreatearea();
	afx_msg void OnLbnSelchangePsPointList();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedPsCheckcreateallarea();
	afx_msg void OnBnClickedPsReflashedge();
	afx_msg void OnBnClickedPsModifyedge();
	afx_msg void OnBnClickedPsCreateNavgrid();
	afx_msg void OnBnClickedPsShowNav();
	afx_msg void OnBnClickedPsTestnavpath();
};

extern CPathSearchDlg* g_CPathSearchDlg;
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
	// ����������������.
	//
	unsigned long m_ulCurRegionId;		// ��ǰ�༭������id.
	//unsigned long m_ulCurPointId;		// ��ǰ�༭�ĵ��id.
	unsigned long m_ulCurLineId;		// ��ǰ�༭���ߵ�id.

	bool		  m_bIsChangeCamera;	    // �Ƿ�ı������λ�á�
	bool		  m_bIsEditingRegion;		// �Ƿ����ڱ༭����

	//int			  m_iCurSelRegionIndex;

public:
	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// ��������
	//

	// ��������༭����
	void SetActive(bool bShow);

	///////////////////////////////////////////////////////////////////////////////////////////
	//
	// �߼����ݲ���
	//

	// ���һ������.
	void AddNewRegion();

	// ɾ��һ������
	void DelRegion();

	// �ڵ�ǰ���������һ���µĵ�
	void RegionAddNewPoint(unsigned long ulId, bool bRefresh = false);

	// ����ǰѡ�е�����
	void DoSelRegion();

	// ���������
	void UpdateCamera();

	// ���һ������ı༭
	bool FinishRegion(unsigned long id);

	// ��鲢�Ҵ�������ѡ��
	bool CheckAndCreateAllRegion();

	///////////////////////////////////////////////////////////////////////////////////////////
	//
	// ����߼�����
	//

	// ѡ�е�Ĳ���
	void DoSelPoint();

	// �ѵ����Ϣˢ�µ�������
	void RefreshRegionPointData(unsigned long ulRegionId);

	// ɾ�������е�һ����
	void DelRegionPoint(unsigned long ulPointId);



	//////////////////////////////////////////////////////////////////////////////////////////
	//
	// ����ص���������
	//

	//������������ص�����
	virtual void onRegionAddNewPoint(unsigned long ulRegionId,unsigned long ulPointId, Ogre::Vector3 position);

	// ɾ��һ����Ļص�����
	virtual void onRegionDelPoint(unsigned long ulRegionId,unsigned long ulPointId);

	// ѡ��һ������
	virtual void onRegionSel(unsigned long ulRegionId);

	//////////////////////////////////////////////////////////////////////////////////////////
	//
	// �����������
	//
	Ogre::ManualObject*	mNavMeshLine;

	SearchSystem::NavSearchSystem mNss;

	//��ȡ��������ڵ�
	Ogre::SceneNode* getNavMeshLineNode();

	//���µ���������
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
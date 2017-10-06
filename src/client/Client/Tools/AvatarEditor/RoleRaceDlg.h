#pragma once


#include "GraphicsSystem.h"


// CRoleRaceDlg dialog

class CRoleRaceDlg : public CDialog
{
public:
	class GraphListener : public GraphicsSystem::Listener
	{
	public:
		void setOwner( CRoleRaceDlg* p ) { pOwner = p; }
		void notifySystemSetup();

	protected:
		CRoleRaceDlg* pOwner;
	};

public:
	CRoleRaceDlg(CWnd* pParent = NULL);   
	virtual ~CRoleRaceDlg();

// Dialog Data
	enum { IDD = IDD_ROLE_RACE_DLG };
public:
	/** 设置角色类型
		@param roleID: 角色类型id
	*/
	void setRoleRace( uint roleID );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedMoTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMSetfocusMoTree(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CTreeCtrl m_wndRoleTree;
	GraphListener	m_graphLis;
};

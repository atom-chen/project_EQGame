#pragma once


// CRoleSectionDlg dialog
#include "AvatarItemDlg.h"
#include "afxwin.h"



/** 角色部件相关
*/
class CRoleSectionDlg : public CDialog
{
public:
	CRoleSectionDlg(CWnd* pParent = NULL);   
	virtual ~CRoleSectionDlg();

// Dialog Data
	enum { IDD = IDD_ROLE_SECTION_DLG };

public:
	void resetContent();

	/** 设置分类的装备名称
	@param bigType 大分类
	*/
	void setLabelText( ItemEquipBigType bigType, const std::string & text );

	void notifyAvatarCreate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAvUpperouter();
	afx_msg void OnBnClickedAvLowergarment();
	afx_msg void OnBnClickedAvFoot();
	afx_msg void OnBnClickedAvSuit();
	afx_msg void OnBnClickedAvHat();
	afx_msg void OnBnClickedAvOrnament();
	afx_msg void OnBnClickedAvBag();
	afx_msg void OnBnClickedAvLefthand();
	afx_msg void OnBnClickedAvNose();
	afx_msg void OnBnClickedAvBack();
	afx_msg void OnDeltaposSpinHair(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHead(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBeard(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinSkin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHairColor(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CAvatarItemDlg	m_wndItemDlg;
private:
	CStatic			mLabelOrnament;
	CStatic			mLabelNose;
	CStatic			mLabelHand;
	CStatic			mLabelHat;
	CStatic			mLabelBack;
	CStatic			mLabelShoes;
	CStatic			mLabelPants;
	CStatic			mLabelCoat;
	CStatic			mLabelBeard;
	CStatic			mLabelSkin;
	CStatic			mLabelHead;
	CStatic			mLabelHairColor;
	CStatic			mLabelHair;
	CStatic			mLabelSuit;

	CSpinButtonCtrl mSpinButtonHairColor;
	CSpinButtonCtrl mSpinButtonHair;
	CSpinButtonCtrl mSpinButtonHead;
	CSpinButtonCtrl mSpinButtonSkin;
	CSpinButtonCtrl mSpinButtonBeard;
};

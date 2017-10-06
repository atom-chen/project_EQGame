#pragma once
#include "afxwin.h"
#include "afxcmn.h"
struct ItemEquipRow;
enum ItemEquipBigType;

class CAvatarItemDlg : public CDialog
{
public:
	CAvatarItemDlg(CWnd* pParent = NULL);   
	virtual ~CAvatarItemDlg();

	enum { IDD = IDD_AVATARSUB };
public:
	void resetContent();
	/** ���¼�����Ʒ�б�
	*/
	void reloadItems();

	/** ������������Ʒ�б�
	*/
	void loadItems( ItemEquipBigType bigType );
	void showItems( ItemEquipBigType bigType );
protected:
	void clearSearch();
	std::string getFilterStr() const;
	std::vector<uint> getSmallTypes() const;
	/** �������ͺ������ִ� ������Ʒ
	*/
	void filterItems( const std::vector<uint> & smallTypes, const std::string & containsStr );
	void doSelectItem();
	void addItem( ulong id, const std::string & name );
	void innerTakeOff( uint takeOffSlots );
	void innerEquipOn( uint equipOnSlots, const ItemEquipRow & item );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual void OnCancel();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAvsTakeoff();
	afx_msg void OnLbnDblclkAvsItemlist();
	afx_msg void OnLbnSelchangeAvsItemlist();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CListBox	m_wndItemList;
	ItemEquipBigType	mCurrBigType;	// ��ǰ�����
	CButton		m_btnClear;				// �����ť
	CEdit		m_edtSearch;			// �༭��
	CTreeCtrl	m_typeTree;				// С�����б�
};

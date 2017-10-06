#pragma once
#include "afxcmn.h"

#include "TerrainPaintInfoContainer.h"
#include "afxwin.h"
#include "resource.h"

class Action;

// CBrushSelectorDlg dialog

class CBrushSelectorDlg : public CDialog
{
	DECLARE_DYNAMIC(CBrushSelectorDlg)

	/// ����ͼƬ���ڵ����·����TreeCtrl�еĽڵ�֮��Ķ�Ӧ��ϵ��������·����һ����ͼƬ���������·��
	/// ����·�����£�a/b/c.jpg����ô���map�оͻᱣ��������Ԫ�أ��ֱ�Ϊa<-->id1��b<-->id2��c.jpg<-->id3
	/// ���ԣ����û�ѡȡ����ͼʱ�����ô����������·�������a/b/c.jpg
	typedef std::map< Ogre::String, HTREEITEM> CatalogMap;

public:
	CBrushSelectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrushSelectorDlg();

// Dialog Data
	enum { IDD = IDD_BRUSH_SELECTOR };

public:
	void ReloadTextureList();
	void unselectTerrainModifyCtl();
	void unselectBrushesTree();

protected:
	HTREEITEM GetParentId( Ogre::String &catalog, CatalogMap &map );

	void buildPreviewBitmap( const TextureInfo& texInfo );

	uchar getBlackWhiteValue(int w, int h);

	void initImage();

	void OnCheck(int cid);

	void BmpRotate90( CBitmap& bitmap, bool clockwise = true );
	void BmpMirror( CBitmap& bitmap, bool horizontally = true );

	const Action* OnStartAutoTexAction( const HTREEITEM& itemId, const std::string& brushName );


protected:
	/// Ԥ��ͼ��С
	const int mPreviewImageWidth;
	const int mPreviewImageHeight;

	CBitmap	mCurrentPreviewImage;
	CBitmap	mBlackWhitePreviewImage;

	/// ��ǰ�����ת��
	int mCurrentOrientationType;
	/// �Ƿ�ʹ������ķ���
	bool mIsRandomOrientation;

	/// ��ǰ���������
	std::string mCurrentFileName;


	bool mPaintInOneGrid;

	bool mTerrainModifyChanging;
	bool mBrushesTreeChanging;

public:
	CToolTipCtrl	mAffectRangeTT;
	CToolTipCtrl	mExpValueTT;
	CToolTipCtrl	mTerrainValueTT;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl mBrushesTree;
	CButton mFlipHorizontal;
	CButton mFlipVertical;
	CButton mRotateRightAngle;
	CButton mMirrorDiagonal;
	CButton mRandom;
	CButton mEnablePreview;
protected:
	virtual void OnOK();
public:
//	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
protected:
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBsReloadPixmapList();
	CStatic mBrushesPreview;
	afx_msg void OnTvnSelchangedBsPixmapList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBsFlipHorizontal();
	afx_msg void OnBnClickedBsFlipVertical();
	afx_msg void OnBnClickedBsRotateRightAngle();
	afx_msg void OnBnClickedBsRmirrorDiagonal();
	afx_msg void OnBnClickedBsRandom();
	afx_msg void OnBnClickedBsEnablePixmapPreview();
	CComboBox mTerrainModifyCtl;
	afx_msg void OnCbnSelchangeBsTerrainModify();
	afx_msg void OnCbnSelchangeBsBrushType();
	CEdit mTerrainValueCtl;
	afx_msg void OnEnChangeBsTerrainValue();
	CEdit mAffectRangeCtl;
	CEdit mExpValueCtl;
	afx_msg void OnEnChangeBsAffectRange();
	afx_msg void OnEnChangeBsExpValue();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CButton mPaintOneGridCtl;
	afx_msg void OnBnClickedBsPaintOneGrid();
};


extern CBrushSelectorDlg* g_CBrushSelectorDlg;
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

	/// 保存图片所在的相对路径与TreeCtrl中的节点之间的对应关系，这个相对路径不一定是图片的整个相对路径
	/// 比如路径如下：a/b/c.jpg，那么这个map中就会保存有三个元素，分别为a<-->id1，b<-->id2，c.jpg<-->id3
	/// 所以，在用户选取纹理图时，还得从下往上组合路径，组成a/b/c.jpg
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
	/// 预览图大小
	const int mPreviewImageWidth;
	const int mPreviewImageHeight;

	CBitmap	mCurrentPreviewImage;
	CBitmap	mBlackWhitePreviewImage;

	/// 当前纹理的转向
	int mCurrentOrientationType;
	/// 是否使用随机的方向
	bool mIsRandomOrientation;

	/// 当前纹理的名称
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
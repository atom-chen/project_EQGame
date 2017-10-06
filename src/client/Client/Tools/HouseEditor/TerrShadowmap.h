#pragma once

#include "Terrain.h"
#include "afxwin.h"

// CTerrShadowmap dialog

class CTerrShadowmap : public CDialog
{
	DECLARE_DYNAMIC(CTerrShadowmap)

public:
	CTerrShadowmap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTerrShadowmap();

// Dialog Data
	enum { IDD = IDD_TERR_SHADOWMAP };

public:
	class PixelBox : public Ogre::PixelBox
	{
	public:
		PixelBox(void)
		{
			data = NULL;
		}

		PixelBox& operator =(const Ogre::Box& box)
		{
			left    = box.left;
			top     = box.top;
			right   = box.right;
			bottom  = box.bottom;
			front   = box.front;
			back    = box.back;
			return *this;
		}
	public:
		Ogre::Rectangle mRectangle;
		float           mScale;
		Ogre::Matrix4   mViewProjMatrix;
		Ogre::Vector3   mCenter;
		Ogre::String    mName;
		bool            mIsNeedRemove;
	};

public:

protected:
	float IntersectObjects(Ogre::Viewport* pViewport, Ogre::HardwarePixelBuffer* pPixelBuffer,
		const Ogre::Ray& ray, const Ogre::RaySceneQueryResult& queryResult);

	bool PrepareLights();

public:
	bool						mIsBitmapNeedUpdate;

	Terrain::LightmapQuality	mLightmapQuality;

	std::vector<Ogre::Vector4>  mLightPositions;
	std::vector<float>          mLightColors;

	Ogre::Image                 mLightmapImage;
	bool                        mIsProcessingLightmap;

	HashMap<Ogre::String, PixelBox> mPixelBoxMap;
	std::vector<PixelBox*>          mPixelBoxPointerVector;

	bool                        mIsLightmapUpdated;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTsmUpdateShadowmap();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedTsmOpt1();
	afx_msg void OnBnClickedTsmOpt2();
	afx_msg void OnBnClickedTsmOpt3();
	CEdit mDensityCtl;
};

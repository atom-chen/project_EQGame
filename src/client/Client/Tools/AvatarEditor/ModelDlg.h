#pragma once

#include "GraphicsSystem.h"
#include "HouseTable.h"
#include "GraphicsTable.h"
#include "HouseSceneComponent.h"
#include "HouseComponentGridGraphics.h"

#define COMPONENT_LIGHT		13 //������
#define COMPONENT_PASS	14 //��͸��
#define DEFAULT_LIGHT Ogre::ColourValue(0.6f, 0.6f, 0.6f) //Ĭ�ϻ�����

class CModelDlg : public CDialog
{
public:
	typedef std::map<std::string, uint> PropItemMap;
	typedef std::map<uint, PropItemMap> PropSectionMap;
public:

public:
	CModelDlg(CWnd* pParent = NULL);   
	virtual ~CModelDlg();

	enum { IDD = IDD_MODEL_DLG };
public:
	void showXml();

	void loadPage( int iPage );
	
	void resetContent();

	void syncNodeInfo();
protected:
	/** ���ģ������
	*/
	void clearProperty();

	/** ��ʾģ������
	*/
	void showModelProperty( const ModelDisplayRow * r );
	
	/** ��ʾ���ݲ���
	*/
	void showHouseCompProperty( uint HouseCompID );

	void showHouseCompProperty();

	void updateHouseData();

	void updateModelData();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusMoTree(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	CTreeCtrl		m_wndModelTree;
	CEdit			m_input;
protected:
	PropSectionMap	mPropSections; // ������¼���Զ�Ӧ�ľ��

	HouseCompRow		mHouseCompRow;
	HouseCompPropRow	mHouseCompPropRow;
	ModelDisplayRow		mModelDisplayRow;
	
	HouseComponentGridGraphics mCompGridGraphics;

	bool mShowHoldGrid;
	bool mShowStopGrid;
	bool mShowLayGrid;
	bool mShowModel;
	bool mShowOffsetHeight;
	//bool mShowXYZ;

protected:
	//������
	Ogre::Light::LightTypes mLightType;
	Ogre::ColourValue mLightDiffuseColour;
	Ogre::ColourValue mLightSpecularColour;
	Ogre::Vector4 mLightAttenuation;
	Ogre::Vector3 mLightPos;
	Ogre::Vector3 mLightDirection;
	Ogre::Vector3 mLightSpotlightRange;
	bool mDrawHelper;
	Ogre::Light* mLight;
	Ogre::BillboardSet* mLightBbs;
	Ogre::Billboard* mLightBb;
	Ogre::ManualObject* mManualobject;
	bool mOpenConsult;
	std::vector<HouseSceneComponent*> mConsultation;

	void resetLight();
	void drawLightHelper(Ogre::Light* light, Ogre::BillboardSet*& bbs, Ogre::Billboard*& bb, Ogre::ManualObject*& manual);
	void drawSphere(Ogre::ManualObject* manual, const float r, const int nRings, const int nSegments);
	void drawCircle(Ogre::ManualObject* manualobject, Ogre::Real radius, Ogre::Real zOffset);
	void addConsultation(int row);
	void clearConsultation();

	//��͸��
	Ogre::Light* mPassLight;
	Ogre::BillboardSet* mPassBbs;
	Ogre::Billboard* mPassBb;
	Ogre::ManualObject* mPassManualobject;
	Ogre::Vector4 mPassAttenuation;
	Ogre::Vector3 mPassPos;
	Ogre::ColourValue mPassVirtualCol;
	Ogre::Real mPassVirtualAtten;
public:
	afx_msg void OnEnChangeEdit1();
};

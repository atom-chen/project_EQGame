#pragma once



class HouseSceneFloor;
class HouseSceneGrid;
class HouseSceneComponent;
class PickObject;
class SceneEntityObject;

class CResourceDlg : public CDialog
{
public:
	typedef std::map<std::string, uint> PropItemMap;
	typedef std::map<uint, PropItemMap> PropSectionMap;
public:
	CResourceDlg(CWnd* pParent = NULL);   
	virtual ~CResourceDlg();
	virtual void OnCancel();

	enum { IDD = IDD_SCENE_DLG };
public:
	void OnAddWall( const HouseSceneGrid * a, const HouseSceneGrid * b );
	void OnRemoveWall( const HouseSceneGrid * a, const HouseSceneGrid * b );
	void OnAddGrid( const HouseSceneFloor * houseFloor, const HouseSceneGrid * grid );
	void OnRemoveGrid( const HouseSceneFloor * houseFloor, const HouseSceneGrid * grid );
	void OnAddComponent( const HouseSceneComponent * comp );
	void OnRemoveComponent( const HouseSceneComponent * comp );
	void selectGrid( HouseSceneGrid * grid );
	void selectComponent( HouseSceneComponent * comp );
	void showSelectGrid();
	void showSelectComponent();
	void showAmbientSetting();
	void showPostProcessSetting();
	void resetContent();
	void notifySceneCreate();
	void notifySceneDestroy();
	void clearProperty();

	void showLightProperty(PickObject* po);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
protected:
	void showCompProperty( const HouseSceneComponent * comp );
	void showGridProperty( const HouseSceneGrid * grid );
	void showFloorProperty( const HouseSceneFloor * floor );
	void loadTree();
private:
	/** 取层高
	*/
	int getTreeItemLayer( HTREEITEM hItem ) const;
protected:
	CTreeCtrl		m_wndResTree;

	PropSectionMap	mPropSections; // 用来记录属性对应的句柄
	PropSectionMap	mAmbientSections;
	PropSectionMap	mLightSections;
	PropSectionMap	mPostProcessSections;

	PickObject*		mLightPo;

	//灯光辅助线
	Ogre::SceneNode* mHelpNode[2];
	Ogre::BillboardSet* mLightBbs[2];
	Ogre::Billboard* mLightBb[2];
	Ogre::ManualObject* mManualobject[2];
	bool mDrawHelp[2];
	void drawCircle(Ogre::ManualObject* manual, Ogre::Real radius, Ogre::Real zOffset);
	void drawSphere(Ogre::ManualObject* manual, const float r, const int nRings, const int nSegments);
	void drawLightHelper(Ogre::Light* light, int n);
	void resetLightHelp(int n);
};

extern CResourceDlg * g_ResourceDlg;
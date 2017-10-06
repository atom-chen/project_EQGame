/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-06
* ����: ȫ�ֵĹ�����, ����һЩ����Ķ���, ͬʱҲ��ʼ����������. ����Ogre::Root
******************************************************************************/
#ifndef RootManager_H
#define RootManager_H

#include "TerrainPaintInfoContainer.h"

typedef std::map<std::string, Ogre::Image *> PreviewImageMap;
class Selection;
class BrushShape;
class HitIndicator;
class Action;
class OperatorManager;
class RegionEditManager;
class TerrainGrid;


class RootManager : public Ogre::Singleton<RootManager> 
{
public:
	RootManager();
	~RootManager();

	void initScene(HWND hWnd);

public:
	// -------------------------------------------
	// General helper
	// -------------------------------------------

	/** Gets a world space ray as cast from the camera through a viewport position.
	@param winx, winy The x and y position at which the ray should intersect the viewport, 
	in window coordinates
	*/
	Ogre::Ray getWindowToViewportRay(Real winx, Real winy) const;

	/** Gets a world space ray as cast from the camera through a viewport position.
	@param pt The x and y position at which the ray should intersect the viewport, 
	in window coordinates
	*/
	Ogre::Ray getWindowToViewportRay(const Ogre::Vector2& pt) const;

	bool getTerrainIntersects(const Ogre::Ray& ray, Ogre::Vector3& position, Ogre::Vector3* normal = 0, bool allowOutside = false) const;
	bool getTerrainIntersects(Real winx, Real winy, Ogre::Vector3& position, Ogre::Vector3* normal = 0, bool allowOutside = false) const;
	bool getTerrainIntersects(const Ogre::Vector2& pt, Ogre::Vector3& position, Ogre::Vector3* normal = 0, bool allowOutside = false) const;


	//-----------------------------
	OperatorManager* getOperatorManager(void) const { return mOperatorManager; }
	RegionEditManager* getRegionEditManager(void) const { return mRegionEditManager; }

	Action* setActiveAction(const std::string& actionName = "", bool active  = true);
	
	void clearActiveAction();

	bool isHitPointShown(void) const    { return mHitPointShown; }
	void showHitPoint(bool show);

	/** Gets the indicator scene node.
	@remark
	All editor special renderables, movables, scene nodes MUST be attach under this node
	*/
	Ogre::SceneNode* getIndicatorRootSceneNode(void) const  { return mIndicatorRootSceneNode; }

	/// ����ָʾ���ڵ�Ŀɼ���
	void setIndicatorNodeVisible(bool visible);
	bool getIndicatorNodeVisible(void)	{		return mIndicatorNodeVisible;	}


	HitIndicator* getHitIndicator(const std::string& name) const;

	BrushShape* _getBrushShape(const std::string& type) const;
	Selection* _getSelection(const std::string& type) const;


	void setBrush(const std::string& type, size_t xsize, size_t zsize);
	const std::string& getBrushType(void) const  { return mBrushType; }
	size_t getBrushXSize(void) const        { return mBrushXSize; }
	size_t getBrushZSize(void) const        { return mBrushZSize; }
	void setBrushXSize(size_t size)         { mBrushXSize = size; }
	void setBrushZSize(size_t size)         { mBrushZSize = size; }
	void _buildSelection(Selection* selection, Real xcentre, Real zcentre);
	void _buildSelection(const std::string& selectionType, Real xcentre, Real zcentre);


	Real getBaseScale(void) const       { return 80.0f; }

	void setHeightAdjustSpeed(Real value)   { mHeightAdjustSpeed = value; }
	Real getHeightAdjustSpeed(void) const   { return mHeightAdjustSpeed; }
	Real _getHeightAdjustSpeed(void) const  { return mHeightAdjustSpeed * getBaseScale(); }


	/// ���øı���θ߶ȵ�Ӱ�췶Χ
	void setJunctionWeightAffectRange( int range )	{	mAffectRange = range;	}
	int getJunctionWeightAffectRange(void) {	return mAffectRange; }

	/// ���øı���θ߶�ʱ���õ�kֵ
	void setJunctionWeightExpValue( float exp ) {	mExponentValue = exp; }
	float getJunctionWeightExpValue(void)	{	return mExponentValue;	}


	/// ��д�����Ƿ�͸������Ϣ
	void AddTransparentTexture( const std::string &textureName, bool transparent = true );
	/// �жϸ������Ƿ���͸����
	bool IsTransparentTexture( const std::string &textureName );

	/// ���¼��ػ��ʱ�ˢ��Ϣ
	void reloadPaintInfo();

	/// ��ȡ��ˢ����
	const TerrainPaintInfoContainer* getTerrainPaintInfoContainer() { return mTerrainPaintInfoContainer; }

	/// ����brush���ƻ�ȡbrush����
	const TextureInfos& getTextureInfos( const std::string &brushName );

	TextureInfo getTextureInfo(const std::string &brushName, const std::string &textureName);

	/// 
	Ogre::Image* getPreviewImage( const std::string &textureName );

	void clearPreviewImage();


	void OnTerrainCreate();


	TerrainGrid* getTerrainGrid();	

	Ogre::ManualObject* getSceneEdgeLine(){ return mSceneEdgeLine; }
	void setSceneEdgeLine(Ogre::ManualObject* m);
	void reshapeTerrainEdge();

protected:
	void registerHitIndicators(void);
	void registerHitIndicator(HitIndicator* hitIndicator);
	void destoryHitIndicators(void);

	void registerBrushShapes(void);
	void registerBrushShape(BrushShape* brushShape);
	void destoryBrushShapes(void);

	void registerSelections(void);
	void registerSelection(Selection* selection);
	void destorySelections(void);

	void loadTerrainPaintInfo(const std::string& name);
protected:
	Ogre::SceneNode*			mIndicatorRootSceneNode;
	bool						mIndicatorNodeVisible;

	/// ������λ�ˢȨ������Ĳ���
	int							mAffectRange;
	float						mExponentValue;

	/// Ԥ��ͼ����
	PreviewImageMap				mPreviewImageMap;
	/// ��ͼ��Ϣ����
	TerrainPaintInfoContainer*	mTerrainPaintInfoContainer;

protected:

	Real mHeightAdjustSpeed;
	std::string mBrushType;
	size_t mBrushXSize;
	size_t mBrushZSize;

	OperatorManager* mOperatorManager;
	RegionEditManager* mRegionEditManager;

	typedef std::map<std::string, HitIndicator*> HitIndicators;
	HitIndicators mHitIndicators;

	typedef std::map<std::string, BrushShape*> BrushShapes;
	BrushShapes mBrushShapes;

	typedef std::map<std::string, Selection*> Selections;
	Selections					mSelections;


	// Flags
	bool mHitPointShown;

	// ͸�������б������alphaͨ�������������
	typedef std::map<std::string, bool> TransparentTextures;
	TransparentTextures mTransparentTextures;

	TerrainGrid* mTerrainGrid;

	Ogre::ManualObject* mSceneEdgeLine;
};

#endif

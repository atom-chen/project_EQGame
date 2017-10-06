/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-27
* ����: ���ݲ���
******************************************************************************/
#ifndef HouseSceneComponent_H
#define HouseSceneComponent_H

#include "EQMainPrerequisites.h"

#include "ModelGcs.h"
#include "HouseTable.h"
#include "HouseSceneDef.h"
#include "AABBBox.h"
#include "PickObjectProxy.h"

class ComponenetHouseRes;
class HouseSceneGrid;
class HouseSceneFloor;
class HouseSceneComponent;
class ZoneLight;


/** ���ݲ���
@remarks �漰�������ʵ��, �Ƿ��ݵ�ͼ�α���. Ŀǰ��ȱ�ٶ�ճ��������ϵ��¼���װ
*/
class HouseSceneComponent : public PickObject
{
	friend class HSCLayLogic;
public:
	/// ����۲�ģʽ�������Ӱ��
	void calcWatchMode( HouseWatchMode mode );

	/// �ڷ�
	void lay( HouseSceneGrid * grid, int face, int height );

	/// ����
	void take();

	/// ��ȡ����
	int getFace() const { return mFace; }

	/// ��ȡ�߶�
	int getHeight() const { return mBottom; }

	/// �Ƿ������������
	bool isLayed() const { return getAttachGrid() != NULL; }

	HouseSceneGrid * getAttachGrid() { return mHoldGrids[mHoldBox.linearIndex(0,0)]; }
	const HouseSceneGrid * getAttachGrid() const { return mHoldGrids[mHoldBox.linearIndex(0,0)]; }
	/// ��ȡռ�ݵ���������
	const std::vector<HouseSceneGrid *> & getHoldGrids() const { return mHoldGrids; }

	/// �Ƿ�ǽ���
	bool isLayWallObject() const;

	/// �Ƿ��Ŵ�
	bool isDoorOrWin() const;

	/// ���������ת����������
	static void convertGrids( HouseCompPropRow::GridPosArray & arr, int face, int gridX, int gridZ );
public:
	/// ��Ҫֱ�ӵ����������, ����HSCManager������
	HouseSceneComponent(uint uid);
	~HouseSceneComponent();

	const std::string& getClassName();

	/// ������Դ
	void parseRes(ComponenetHouseRes* res, const std::string& addPath = "");

	/// ������Դ
	void parseRes( uint compId );

	/// ������Դ
	void parseRes( const HouseCompRow * compRow, const HouseCompPropRow * propRow );

	/// д����Դ 
	void writeRes(ComponenetHouseRes * res);
	
	/// ����
	void load();

	/// ��ȡΨһid
	uint getUid() const { return mUid; }

	/// ��ȡģ��.
	ModelGcs* getModel() { return mModel; }
	const ModelGcs* getModel() const { return mModel; }

	/// ��ȡ�ҽӵƹ�
	std::vector<ZoneLight*>& getLight() { return mLights; }
	void turnOnLight();
	void turnOffLight();

	int getFloorNO(){return mFlr;}
	uint getRoomNO(){return mRoom;}
	
public:
	bool isFromFrame() const { return mIsFromFrame; }

	/// �������
	const std::string & getFrmMesh() const { return mFrmMesh; }
	const Ogre::Vector3 & getFrmPos() const { return mFrmPos; }
	const Ogre::Quaternion & getFrmRot() const { return mFrmRot; }
	const Ogre::Vector3 & getFrmScale() const { return mFrmScale; }
	const HouseCompPropRow * getHouseCompPropRow() const {	return mHouseCompPropRow; }
	const HouseCompRow * getHouseCompRow() const {	return mHouseCompRow; }

	/// �ײ�(y����)
	int getBottom() const;

	/// ����(y����) 
	int getTop() const;

	const Ogre::Vector3 & getPosition() const;
	const Ogre::Quaternion & getOrientation() const;

	/// ����λ��
	void setPosition( const Ogre::Vector3 & pos );

	/// ������ת
	void setOrientation( const Ogre::Quaternion & rot );

	/// ���ø��ڵ�
	void setParentNode( Ogre::SceneNode * parentNode );

	/// ��ȡ��Դ����
	const std::string & getResName() const { return mResName; }

	/// ������Դ��
	void setResName(const std::string & n) { mResName = n; }

	/// ����ʹ������
	std::set<const HouseSceneGrid*>& calcUseGrids(Ogre::uchar face, uint range);

	/// ����Ҿ߽�����Ϣ
	std::vector<int>& calcInteractMorph(int pos, int flag, int user_count);

protected:
	void setVisible(bool vi);

	void _removeFromScene();

	/// �뿪����
	void unLay();
	void _updatePosAndRot();

	/// ����ռ������
	std::vector<const HouseSceneGrid *> _calcHoldGrids( const HouseSceneGrid * attachGrid, int face ) const;

	bool _checkPassLights(bool& forward_indoor);

	void createSheetShadow();
	void destroySheetShadow();

private:
	HouseSceneComponent();

	void _initProperty();

protected: //����
	/// ����, ����Դ����ʱʹ��.���������Դ��Ψһ��, �Ժ�,����������������Ҫ�����Ϣ
	std::string				mResName;

	/// �Ƿ��ǿ��
	bool					mIsFromFrame;

	/// ���Ͷ�Ӧ�ı������
	const HouseCompPropRow * mHouseCompPropRow;

	/// ����
	int						mFace;

protected: //���������������Լ�
	/// ģ������
	std::string				mFrmMesh;

	/// ��ʼ����
	Ogre::Vector3			mFrmPos;

	/// ��ʼ����ת
	Ogre::Quaternion		mFrmRot;

	/// ��ʼ������
	Ogre::Vector3			mFrmScale;

protected: //���ݲ�������������Լ�
	/// ���ݲ���������
	const HouseCompRow *	mHouseCompRow;

protected:
	//����Ѿ��ڷ�, �������Щ����֮��.
	//����Ƿ��ڱ���������, ��ָ����Ӧ�����

protected:
	/// ΨһID
	uint					mUid;

	/// �Ƿ����
	bool					mLoaded;

	/// ռ�ݸ��box
	Int2AABBBox				mHoldBox;

	/// ռ����������
	std::vector<HouseSceneGrid *>	mHoldGrids;

	/// ʹ������
	std::set<const HouseSceneGrid *> mUseGrids;

	/// �Ҿ߽�����Ϣ
	std::vector<int>		mInteractMorph;

	///// ���ӵ�����
	//HouseSceneGrid*			mAttachGrid;

	/// ���ӵ�·��
	std::string				mAddPath;

protected:
	/// �����λ����Ϣ
	Ogre::Vector3			mPos;
	Ogre::Quaternion		mRot;

	/// ��Ӧ��ģ��ʵ��
	ModelGcs *				mModel;

	/// ����Ӱ
	Ogre::ManualObject*		mSheetShadow;
	Ogre::SceneNode*		mSheetShadowNode;

	/// ѡȡ����
	PickObjectProxy			mProxy;

protected:
	/// �ײ��ĸ߶� 
	int						mBottom;

protected:
	/// ���ռ�ݵ�����
	std::vector<uint>		mHoldZone;
	/// Я����Դ
	std::vector<ZoneLight*>	mLights;
protected:
	/// ����¥��
	int						mFlr;
	/// ���ڷ���
	uint					mRoom;
};



/** ���ݲ����ڷ��߼�
*/
class HSCLayLogic
{
public:
	/// ��ĳ���������ҿ����滻�����
	static HouseSceneComponent * findCanReplaceComponent( const HouseSceneComponent * pThis, const HouseSceneGrid * grid );

	/// �ж�һ������Ƿ����滻��һ�����
	static bool canReplaceComponent( const HouseSceneComponent * pThis, const HouseSceneComponent * dst );

	/// �ܷ�ڷ�
	static HouseReturnValue canLay( const HouseSceneComponent * pThis, const HouseSceneGrid * grid, int face, int & height );

	/// �Զ���ǽ
	static HouseReturnValue canAutoLayWall( const HouseSceneComponent * pThis, const HouseSceneGrid * grid, int & face, int & height );

	/// �ܷ�����
	static HouseReturnValue canTake( const HouseSceneComponent * pThis );

	static std::pair<bool, const HouseSceneComponent *> getFirstCanLayComponent( const HouseSceneComponent * pThis, const HouseSceneGrid * grid );
};


#endif

/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-27
* ����: ����¥��
******************************************************************************/
#ifndef HouseSceneFloor_H
#define HouseSceneFloor_H

#include "EQMainPrerequisites.h"

#include "HouseSceneGrid.h"
#include "CompareVectorTemplate.h"

/** ����¥��
@remarks �Ժ�����д���԰, ����Ҳ����Ϊһ��. (�������ʱ,¥����ò�Ҫ��0,1,2������ʶ)
	�Ժ�¥����ܻ����������Ϣ.
*/
class HouseSceneFloor
{
	friend class HouseScene;
public:
	typedef CompareVector2Template<int> GridNo;
	typedef std::map<GridNo, HouseSceneGrid*> GridMap;
public:
	/// ��ȡ��ǰ��߶�
	int getHeight() const;

	/// ����¥��߶�. ֻ��û���ӵ�ʱ���������¥��߶�. �����쳣
	void modifyFloorHeight( float h );

	/// ��ȡ��ǰ��¥����
	int getFloorNO() const { return mFloorNO; }

	/// ����������ʾģʽ
	void calcGridDisplayMode(GridDisplayMode mode);

	/// ��ȡ�򴴽����ݵĸ�����, ���������Ϊ���ݿ��׼����.
	const HouseSceneGrid * getRootGrid() const;
	HouseSceneGrid * getRootGrid();

	/// ��������
	HouseSceneGrid* createGrid( int x, int z, ushort typeID, ushort roomID=0 );

	/// ��ȡ����
	HouseSceneGrid* getGrid( int x, int z );
	const HouseSceneGrid* getGrid( int x, int z ) const;

	/// �Ƿ��ж�Ӧ�ĸ���
	bool hasGrid( int x, int z ) const;

	/// �Ƿ�������
	bool hasGrids() const;

	/// ɾ������
	void destroyGrid( int x, int z );

	const GridMap & getGridMap() const { return mGridMap; }

	/// ���ӱ���ɾ�����ı�
	void notifyGridChanged();

	/// �����ű�����
	void notifyRoomIDChanged();
public: // ͼ��
	//Ogre::SceneNode * getSceneNode() { return mSceneNode; }
	/// ����۲�ģʽ, ���ܻ��޸�
	void calcWatchMode( HouseWatchMode mode );

	/// ��ȡ¥��߶�(�����0�߶�)
	int getFloorHeight() const { return mFloorHeight; }
public:
	HouseSceneFloor(HouseScene* scene);
	~HouseSceneFloor();

	/// ��ȡ������
	HouseScene* getCreator() { return mCreator; }

	/// ������һ��
	void setUpFloor(HouseSceneFloor* up);

	/// ��ȡ��һ��
	HouseSceneFloor* getUpFloor() { return mUpFloor; }
	const HouseSceneFloor* getUpFloor() const { return mUpFloor; }

	/// ������һ��
	void setDownFloor(HouseSceneFloor* down);

	/// ������һ��
	HouseSceneFloor* getDownFloor() { return mDownFloor; }
	const HouseSceneFloor* getDownFloor() const { return mDownFloor; }

	/// ֻ���ڱ༭������ʾ�������ڷ���ID
	void filterByRoomID( int roomid );
	void showRoomIDPlane( bool bShow );
private:
	void _showGridTypePlane( bool bShow );

	void _showWallPlane( bool bShow );

	void _showHoldGridPlane( bool bShow, bool bFitmentMode = false );

	void _showRoomIDPlane( bool bShow, bool bCareID, int filterID );
private:
	HouseSceneFloor();

protected:
	HouseScene*					mCreator;

	/// ������, ÿ������������1��������. ��Ŀǰ��ҪΪ��max�����ķ��ݿ��׼����׼����. ����ǻ�԰��Ӧ��û�����Ϣ
	HouseSceneGrid*				mRootGrid;

	/// ���ӱ� ������Ҫ�Ż�,��ʱ������ά����
	GridMap						mGridMap;

	/// ¥��߶�
	int							mFloorHeight;

	/// ��ǰ�ڼ������
	int							mFloorNO;

	/// ��һ��
	HouseSceneFloor*			mUpFloor;

	/// ��һ��
	HouseSceneFloor*			mDownFloor;
protected: // ͼ��
	/// �����������
	Ogre::ManualObject *		mGridTypePlane;

	/// ǽ���������
	Ogre::ManualObject *		mWallPlane;

	/// ����ģ��
	Ogre::ManualObject *		mHoldGridPlane;

	/// ���������
	Ogre::ManualObject *		mRoomIDPlane;

	/// 
	Ogre::SceneNode *			mSceneNode;

/// ¥�㲿������
public:
	/// ¥�㷿���ڵ�����������������ƹ⣬������������
	struct RoomComponent
	{
		RoomComponent():floor(NULL),tcx(NULL){}
		HouseSceneComponent* floor;
		std::vector<HouseSceneComponent*> wall;
		std::vector<HouseSceneComponent*> tcx;
		std::vector<HouseSceneComponent*> sta;
		std::vector<HouseSceneComponent*> comp;
	};
	typedef std::map<uint, RoomComponent> RoomComponentMap;

	void insertRoomComponent(uint room, HouseSceneComponent* comp);
	void removeRoomComponent(uint room, HouseSceneComponent* comp);

	void changeRoomFloorMaterial(uint room, std::string& floormat);
	void changeRoomWallMaterial(uint room, std::string& wallmat);
	void changeRoomTcxMaterial(uint room, std::string& tcxmat);

	// ��ȡ���ݲ����Ͷ�Ӧ�Ĳ��� (2011/7/25 add by hezhr)
	RoomComponent* getRoomComponent(uint room);
	std::string getRoomFloorMaterial(uint room);
	std::string getRoomWallMaterial(uint room);
	std::string getRoomTcxMaterial(uint room);

private:
	RoomComponentMap mRoomComponentMap;
};


#endif

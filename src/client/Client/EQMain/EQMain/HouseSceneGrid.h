/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-27
* ����: ���ݵĸ�����Ϣ
******************************************************************************/
#ifndef HouseSceneGrid_H
#define HouseSceneGrid_H

#include "EQMainPrerequisites.h"

#include "HouseSceneComponent.h"

struct HouseGridTypeRow;


/** ���ݵĸ���
@remarks �Ժ�����д���԰, ��԰�ĸ��ӿ��ܻ���Ҫ��չ, ���߲�������. 
@par ���Կ����ȰѸ����ϵ�����ƿ�, �����޸����Ը���
*/
class HouseSceneGrid
{
public:
	typedef std::list<HouseSceneComponent *> ComponentList;
public:
	/// 
	bool hasComponents() const { return !mHouseSceneComponentList.empty(); }

	void attachComponent( HouseSceneComponent * com );
	void detachComponent( HouseSceneComponent * com );

	/// �Ƿ����ھ�����, ͬʱ���س���
	bool isNeighborGrid( const HouseSceneGrid * grid, int & face ) const;

	/// �����Ƿ���
	void frameGrid( bool b );

	/// �Ƿ��ǿ�ܸ���
	bool isFrameGrid() const { return mIsFrameGrid; }

	/// ��ȡ������
	void getGridNo( int& x, int& z ) const { x = mGridX; z = mGridZ; }
	int getGridX() const { return mGridX; }
	int getGridZ() const { return mGridZ; }

	/// ����������
	void setGridNo( int x, int z );

	/// ���ø�������, ֻ������һ��
	void setGridType( ushort typeID );

	/** ����������ǽ, ����˫������Ϊ, ��������һ��Ӱ�������, ������������.
	remarks face 0Ϊx������. ��ʱ�뷽���ȡ,��, 0,x  1,-z  2,-x  3,z
	*/
	HouseSceneGrid* setWallFace( int face, bool wall );

	/// ��ȡǽ�����
	bool getWallFace( int face ) const { return mWallFace[face]; }

	/// ��ȡ������ھ���������
	void getFaceGridNo( int face, int& x, int& z ) const;

	bool hasWallFace() const;

	/// ��Ϊ�赲
	void setBlocked( bool bVal );

	/// �ж��Ƿ��赲
	bool isBlocked() const { return mBlocked; }

	const HouseGridTypeRow * getHouseGridTypeRow() const { return mHouseGridTypeRow; }

	void setRoomID(int id) { mRoomID = id; }
	Ogre::uint getRoomID() const { return mRoomID; }
	Ogre::SceneNode* getSceneNode();	
	const Ogre::SceneNode* getSceneNode() const;	

	/// ����۲�ģʽ, ���ܻ��޸�
	void calcWatchMode( HouseWatchMode mode );

protected:
	/// ��ȡ������
	static int negativeFace( int face );
public:
	HouseSceneGrid(HouseSceneFloor* floor);
	~HouseSceneGrid();

	HouseSceneFloor* getCreator() { return mCreator; }
	const HouseSceneFloor* getCreator() const { return mCreator; }


	const ComponentList & getHouseSceneComponentList() const { return mHouseSceneComponentList; }
	//��Ӧһ�����Ŀǰ�����˼�������

	//��ȡ��ǰ����ֱ�ӷŵ�����б�
	////��ȡ���ӵ���������б�, ��������������
private:
	HouseSceneGrid();

	void _updatePosition();	
	void _exceptionIfHasComp();
protected:
	// �����ĸ�¥��
	HouseSceneFloor*	mCreator;

protected:	//��������
	/// ��������
	const HouseGridTypeRow * mHouseGridTypeRow;

	/// �ǲ��ǿ�ܸ���
	bool				mIsFrameGrid;

	/// �赵����
	bool				mBlocked;

	/// ���ӱ��
	int					mGridX;
	int					mGridZ;

	/// ������ǽ��, ͬʱҲ���ٽ����Ӳ�����ͨ����(Ŀǰ��). 0Ϊx������. ��ʱ�뷽���ȡ
	bool				mWallFace[WALL_FACE_COUNT];

protected:	//���
	//�ɰڷ�����-----------------------
	/// �Ѱڷŵ��б�
	ComponentList		mHouseSceneComponentList;

	/// �������ڷ�����
	Ogre::uint			mRoomID;
protected:	//ͼ�����
	/// ���Կ����Ż�Ϊ, û��childʱ������HouseScene�ڵ�
	Ogre::SceneNode*	mSceneNode;
};


#endif

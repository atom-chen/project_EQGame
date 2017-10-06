/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-27
* ����: ����. 
******************************************************************************/
#ifndef HouseScene_H
#define HouseScene_H

#include "EQMainPrerequisites.h"

#include "HouseSceneDef.h"
#include "HouseSceneFloor.h"
#include "HouseSceneComponent.h"
#include "HouseRes.h"
#include "HSCFitmentCtrl.h"



class HouseSceneSerializer;

/** ����
@remarks 
	���ݵĳ�������, ��༭�����Ժ���Ϸ�еļ�װģʽ�н������. �����, ���丽����,
	����߼����Ŀǰ��λ�ڴ����߼����ͼ�α��ֲ�֮��, ��Ȼ����2���и��ӵ���ϵ.
	�������δ�غܺ���, ��ʵ��û�и��õ��뷨.
@par
	���ݳ����Ļ����ṹ��: ���� (����) ¥�� (����) ���� (����) ����
@par
	Ŀǰ���߼���һ������ֻ����һ��HouseScene. ���仰, ����HouseScene������Ϸ����.
*/
class HouseScene
{	
	class HouseScenePairGrid : public std::pair<const HouseSceneGrid *, const HouseSceneGrid *>
	{
	public:
		HouseScenePairGrid( HouseSceneGrid * a, HouseSceneGrid * b )
			: std::pair<const HouseSceneGrid *, const HouseSceneGrid *>( a, b )
		{
			if ( this->first > this->second )
			{
				std::swap( this->first, this->second );
			}
		}
	};
public:
	friend class HouseSceneSerializer;
	typedef HouseScenePairGrid PairGrid;
	typedef std::set<PairGrid> PairGridList;
	typedef std::map<PairGrid, Ogre::ManualObject *> PairGridObjectList;

public:	//���õķ���
	HouseScene(Ogre::Camera* cam);
	~HouseScene();

	/// ��������
	void buildHouse();

	/// �Ƿ��Ѿ������˷���
	bool isLoadHouse() { return mHouseLoaded; }

	/// ����������(��԰��)
	//TODO:

	/// ��ȡ��ǰ��
	HouseSceneFloor* getWalkViewFloor() { return mWalkViewFloor; }
	/// ��ȡ��Ͳ�
	HouseSceneFloor* getRootHouseFloor() { return mRootHouseFloor; }

	/// ��ȡ��
	HouseSceneFloor* getFloorByNum(int n);
 
	/** ��ȡray����. 
	@param ray ��������, ray��������ǳ���, ��������-y������������
	@param gr �����ཻ������
	@param worldCrossingPoint �����ཻ���������꽻��
	@param allHouse ǿ����������. true, ���ܶ�Ӧ¥����ʾ�Ƿ����ض�����������Ӽ����; false, ֻ�ӵ�ǰ¥�㿪ʼ���
	*/
	bool rayGrid( const Ogre::Ray& ray, HouseSceneGrid *& gr, Ogre::Vector3 & worldCrossingPoint, bool allHouse = false );

	/** ��ȡ�����Ӧ������
	@param pos ��������
	@param YBias Y���ƫ��. �㾭����������������, ������һ�������, ������Ҫ���ֵ.
	@param NeYBias -Y���ƫ��.
	@return ��������, ���û�ҵ�, ���ؿ�
	*/
	HouseSceneGrid* gridByPosition(const Ogre::Vector3& pos, float YBias = 0.5f, float NeYBias = 0.5f);

	/** ��ȡ��ǰ¥��Ľ���� 
	*/
	void getXZ( int & x, int & z, const Ogre::Ray & ray );

	HouseSceneGrid * getGrid( int flrNO, int x, int z );

	/// ��ȡ��װ�������Ӧ��
	HSCFitmentCtrl& getFitmentCtrl() { return mFitmentCtrl; }

	/// ����۲�ģʽ
	void changeWatchMode(HouseWatchMode mode);

	/// ��ȡ�۲�ģʽ
	HouseWatchMode getWatchMode() const { return mWatchMode; }

	/// ��ȡ��Ӧ¥��Ĺ۲�ģʽ
	HouseWatchMode getWatchModeByFloorNO(int flrNO) const;

	/// ����һ�� 
	bool upView();

	/// ����һ��
	bool downView();

	/// �ı�������ʾģʽ
	void changeGridDisplayMode(GridDisplayMode mode);

	GridDisplayMode getGridDisplayMode() const { return mGridDisplayMode; }


	Ogre::SceneNode* getSceneNode() { return mSceneNode; }

	Ogre::Camera * getCamera() { return mCamera; }

	void update(float delta);

	void turnAllLight(bool on);

public:
	/** �Ƿ���ת��Ϊ��Դ
	@remarks ���� 1.���ݲ���Ҫ������, �Ҳ���ͬ��
	@param reason ����ʧ�ܵ�ԭ��
	*/
	bool canConvertToResource(std::string& reason);

	/** ת������Դ
	@remarks ��ֻ�Ǹ�������Դ, ��������HouseResManager�Ĺ���.
	*/
	HouseResPtr convertToResource();

	const std::map<int, HouseSceneFloor*> & getFloors() const { return mFloors; }
	/// ���Ӳ���������, �ڲ�����
	void _attachComponent(HouseSceneComponent* comp);

	/// �����������, �ڲ�����
	void _detachComponent(uint id);

	/// ��ȡ��������б�
	const std::map<uint, HouseSceneComponent*>& _getComponentMap() const {return mComponentMap;}

	/// ȡ�������б�
	const PairGridList & getPairGridList() const { return mPairGridList; }

	/// ��������
	void addStair( HouseSceneGrid * a, HouseSceneGrid * b );

	/// ɾ������
	void removeStair( HouseSceneGrid * a, HouseSceneGrid * b );

	/// ���ø���ƽ�������(�뾶)
	void setFloorAssistGridNum( int n );

	/// ȡ�ø��������� 
	int getFloorAssistGridNum() const { return mFloorAssistGridNum; }

	/// ��ʾ����ƽ��
	void showAssistPlane( bool bShow );

private:
	/// ¥����ͨ��Ϣ��ͼ�α���
	void addStairGraphics( HouseSceneGrid * a, HouseSceneGrid * b );
	void removeStairGraphics( HouseSceneGrid * a, HouseSceneGrid * b );
	void showStairGraphics( bool bVal );
	void _destroyManualObject( Ogre::ManualObject * obj );
	/// ��������ƽ��
	void _createAssistPlane( bool bVisible );
	/// ���¸���ƽ��߶�
	void _updateAssistPlaneHeight();
protected:
	void setWalkViewFloor( HouseSceneFloor * flr ) { mWalkViewFloor = flr; }
	void setWatchMode( HouseWatchMode mode ) { mWatchMode = mode; }
	void _buildFloors(HouseResPtr house);
	void _buildComps(HouseResPtr house);
	void _initGraphics();
	void _destroyGraphics();
private:
	/// ���������ַ�ʽ
	HouseScene();

protected: 
	/// ¥�������
	std::map<int, HouseSceneFloor *> mFloors;

	/// ����¥����ײ�. �Ƿ����Ϊ��, ����
	HouseSceneFloor *				mRootHouseFloor;

	/// ��ǰ����һ����ʾ, ��ʵ���Կ��������ֵһֱ��Ч. ������ׯ԰��
	HouseSceneFloor *				mWalkViewFloor;

	/// ����ӵ�е����. ����Ĺ�����Ҫ��HouseSceneGrid����. ������Ϊ��˵����������ô�����
	std::map<uint, HouseSceneComponent *> mComponentMap;

	/// ��ͨ��Ϣ����
	PairGridList					mPairGridList;

	/// ��װ����
	HSCFitmentCtrl					mFitmentCtrl;
	/// �۲�ģʽ
	HouseWatchMode					mWatchMode;
	/// ������ʾģʽ
	GridDisplayMode					mGridDisplayMode;

	/// ������Դ��·��
	std::string						mMatPathBak;

	/// �Ƿ��Ѿ������˷�����Դ
	bool							mHouseLoaded;
	/// ������Դ������
	std::string						mHouseResName;

protected: // ͼ��
	Ogre::SceneNode *				mSceneNode;
	Ogre::Camera *					mCamera;	

	/// ��ͼ�ϵ����ӵ�
	PairGridObjectList				mPairGridObjectList;

	/// ����ƽ�����
	Ogre::ManualObject *			mFloorAssistPlane;
	int								mFloorAssistPlaneHeight;
	/// ���ٸ�����
	int								mFloorAssistGridNum;

};

#endif
/******************************************************************************
* ����: Y3
* ʱ��: 2009-11-20
* ����: �ӷ�����Դ
******************************************************************************/
#ifndef SubHouseRes_H
#define SubHouseRes_H

#include "EQMainPrerequisites.h"

class HouseRes;

/** ¥��ķ�����Դ
*/
class FloorHouseRes
{
public:
	/// ���
	int					mNO;

	/// ¥�ϵ�¥��ı��, >255Ϊ��Чֵ
	int					mUpFloorNO;

	/// ¥��ĸ߶�
	int					mHeight;


	/** ������Դ
	*/
	class Grid
	{
	public:
		/// ��������
		ushort			mType;

		///// �ǲ��ǿ������
		//bool			mFrame;	//Ĭ����

		/// ������¥���еı��
		int				mX;
		int				mZ;

		/// ��������������
		ushort			mRoom;

	public:
		Grid();
	};

	/// �����б�
	std::vector<Grid>	mGrids;


	/** ǽ��
	*/
	class Wall
	{
	public:
		/// ǽ���Ӧ�������еı��
		int				mX0;
		int				mZ0;
		int				mX1;
		int				mZ1;

	public:
		Wall();
	};

	/// ǽ���б�
	std::vector<Wall>	mWalls;

public:
	FloorHouseRes();
};


/** ���ݲ�����Դ
*/
class ComponenetHouseRes
{
public:
	/// ����, Ψһ��ʶ
	std::string			mName;

	/// �Ƿ��ǿ�ܲ���
	bool				mFrame;

	/// ��ܲ�������
	ushort				mFrmCompType;

	/// ��ܲ�������
	std::string			mFrmMesh;

	/// ��ܲ����Ŀռ�λ��
	Ogre::Vector3		mFrmPos;
	Ogre::Quaternion	mFrmRot;
	Ogre::Vector3		mFrmScale;

	/// ��ͨ������ӦHouseComp.xml���id
	ulong				mCompId;

	/// �ڷŵ�¥��id
	uint				mFlr;
	/// �ڷŵķ���id
	uint				mRoom;

	/// �ڷ�������ĸ��Ӷ�Ӧ¥����ӵı��
	int					mX;
	int					mZ;

	/// �ڷ�����ĳ���
	int					mFace;

	/// ���ڰڷŵĵڼ���, ����ȷ���ڷ�˳��. 0Ϊ��ײ�
	int					mLayer;

	/// �ڷŵײ��ĸ߶�
	int					mBottom;
public:
	ComponenetHouseRes();
};

/** ���Ӹ���
*/
class LinkGridRes
{
	class Grid
	{
	public:
		int mFloorNo;
		int mX;
		int mZ;
	};
public:
	Grid mStartGrid;
	Grid mEndGrid;
};

#endif




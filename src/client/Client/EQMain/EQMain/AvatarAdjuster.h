#ifndef AvatarAdjuster_H
#define AvatarAdjuster_H

#include "EQMainPrerequisites.h"
#include "ModelManagerGcs.h"


class AvatarAdjuster
{
public:

	/** Avatar ��������
	*/
	enum AvatarAdjustType
	{
		atSex,			// �Ա�
		atHairType,		// ����
		atHairColor,	// ��ɫ
		atFace,			// ����
		atSkin,			// ��ɫ
		atBeard,		// ����
	};

	explicit AvatarAdjuster( Avatar * avt );
public:
	/// ����һ��(���ͣ�����֮��)
	void doNext( AvatarAdjustType adjustType );

	/// ����һ��(���ͣ�����֮��)
	void doPrev( AvatarAdjustType adjustType );

	/// �ı�ĳ��(���ͣ�����֮��)
	void doChange( AvatarAdjustType adjustType, int val );
	
	/// ȡ�ö�Ӧ�����������͵ĵ�ǰֵ
	int	 getCurrentVal( AvatarAdjustType adjustType );

public:
	/// ���¼���Ϊ�༭��ʹ��
	int getSkinColorID() const { return mSkinColor; }
	int getHairColorID() const { return mHairColor; }
	const std::string & getHeadMesh() const { return mHeadMesh; }
	const std::string & getHairMesh() const { return mHairMesh; }
	const std::string & getBodyMesh() const { return mBodyMesh; }
	const std::string & getBeardMesh() const { return mBeardMesh; }
	void reset();
public:
	void setForCreateRole( bool bVal ) { mbForCreateRole = bVal; }
	/// �Ա�
	void changeSex();
	int  getSex(){return mSex;}

	/// ����
	void nextHairType();
	void prevHairType();
	
	/// ��ɫ
	void nextHairColor();
	void prevHairColor();
	
	/// ����
	void nextFace();
	void prevFace();
	
	/// ��ɫ
	void nextSkin();
	void prevSkin();
	
	/// ����
	void nextBeard();
	void prevBeard();	
private:
	void updateHairType();
	void updateHairColor();
	void updateFace();
	void updateSkin();
	void updateBeard();
	void _updateRoleSection(uint type, uint color, int & refPos);
	void _changeRoleSection( uint sectionType, uint color, uint index );
	void _changeRoleSection( uint sectionType, uint modelDisplayID );
	void _changeAvatar();
private:
	/// �ı��ɫ
	void changeSkin( int skinColor );
	void changeHairColor( int hairColor );
private:
	bool						mbForCreateRole; // �Ƿ����ڴ�����ɫ; �ǣ�ֻ��ѡCreateVisible�Ĳ��� �񣺿�ѡ�����в���
	int							mCurHairColorPos;			
	int							mCurSkinColorPos;			
	int							mCurHairPos;
	int							mCurBeardPos;
	int							mCurHeadPos;
	int							mCurBodyPos;
	int							mHairColor;			// ��ǰ��ɫ
	int							mSkinColor;			// ��ǰ��ɫ
	int							mSex;
	std::string					mHeadMesh;
	std::string					mHairMesh;
	std::string					mBodyMesh;
	std::string					mBeardMesh;
	Avatar *					mAvatar;
};

#endif
#ifndef AvatarAdjuster_H
#define AvatarAdjuster_H

#include "EQMainPrerequisites.h"
#include "ModelManagerGcs.h"


class AvatarAdjuster
{
public:

	/** Avatar 调整类型
	*/
	enum AvatarAdjustType
	{
		atSex,			// 性别
		atHairType,		// 发型
		atHairColor,	// 发色
		atFace,			// 脸形
		atSkin,			// 肤色
		atBeard,		// 胡子
	};

	explicit AvatarAdjuster( Avatar * avt );
public:
	/// 换下一个(发型，脸型之类)
	void doNext( AvatarAdjustType adjustType );

	/// 换上一个(发型，脸型之类)
	void doPrev( AvatarAdjustType adjustType );

	/// 改变某个(发型，脸型之类)
	void doChange( AvatarAdjustType adjustType, int val );
	
	/// 取得对应各个调整类型的当前值
	int	 getCurrentVal( AvatarAdjustType adjustType );

public:
	/// 以下几个为编辑器使用
	int getSkinColorID() const { return mSkinColor; }
	int getHairColorID() const { return mHairColor; }
	const std::string & getHeadMesh() const { return mHeadMesh; }
	const std::string & getHairMesh() const { return mHairMesh; }
	const std::string & getBodyMesh() const { return mBodyMesh; }
	const std::string & getBeardMesh() const { return mBeardMesh; }
	void reset();
public:
	void setForCreateRole( bool bVal ) { mbForCreateRole = bVal; }
	/// 性别
	void changeSex();
	int  getSex(){return mSex;}

	/// 发型
	void nextHairType();
	void prevHairType();
	
	/// 发色
	void nextHairColor();
	void prevHairColor();
	
	/// 脸形
	void nextFace();
	void prevFace();
	
	/// 肤色
	void nextSkin();
	void prevSkin();
	
	/// 胡子
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
	/// 改变肤色
	void changeSkin( int skinColor );
	void changeHairColor( int hairColor );
private:
	bool						mbForCreateRole; // 是否用于创建角色; 是：只能选CreateVisible的部件 否：可选择所有部件
	int							mCurHairColorPos;			
	int							mCurSkinColorPos;			
	int							mCurHairPos;
	int							mCurBeardPos;
	int							mCurHeadPos;
	int							mCurBodyPos;
	int							mHairColor;			// 当前发色
	int							mSkinColor;			// 当前肤色
	int							mSex;
	std::string					mHeadMesh;
	std::string					mHairMesh;
	std::string					mBodyMesh;
	std::string					mBeardMesh;
	Avatar *					mAvatar;
};

#endif
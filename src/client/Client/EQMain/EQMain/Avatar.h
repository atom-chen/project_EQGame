/******************************************************************************
* 作者: Hongjx
* 时间: 2009-11-27
* 描述: 换装角色类, 换装逻辑类
******************************************************************************/
#ifndef Avatar_H
#define Avatar_H

#include "EQMainPrerequisites.h"
#include "ModelManagerGcs.h"

class Item;
struct RoleRaceRow;
struct RoleSectionRow;
struct ModelDisplayRow;
struct EquipDisplayRow;
class AvatarAdjuster;
enum EquipSlotType;


/** 换装角色类
	说明: 每个角色都有保证的mModel, mRoleRaceInfo不为空
*/
class Avatar
{
public:
	typedef std::map<uint, std::vector<std::string> >	ShareEntitys;
	typedef std::map<uint, std::vector<std::string> >	HangEntitys;
	typedef std::map<uint, uint>						ExtraHoldSlot;
	typedef std::vector<RoleSectionRow const *>			RoleSectionRows;
	typedef std::map<uint, std::string>					RoleSectionMap;
public:
	/** 角色创建规则
	*/
	class AvatarCreateRule : public ModelGcs::CreateModelRuleGcs
	{
	public:
		explicit AvatarCreateRule( Avatar * pAvatar );
	public:
		virtual void create( ModelGcs * pModel );
	protected:
		Avatar * mAvatar;
	};
public:
	virtual ~Avatar();

	/** 构造函数
		说明: 每个角色都有保证的mModel, mRoleRaceInfo不为空
	*/
	explicit Avatar( uint id );
public:
	/** 装备上某个物品
		@param solt 装备栏, 请参考EquipSlotType
	*/
	void equipOn( uint slot, uint equipDisplayID, uint holdSlot );

	/** 卸下某个装备栏的所有物品
		@param solt 装备栏, 请参考EquipSlotType
	*/
	void takeOff( uint slot );
	void takeOffAll();

	// 为0时用原始发型替换
	void replaceHair(uint repHair);

	/** 隐藏角色部件
	*/
	void hideSlot( uint slotArr );
	void restoreSlot();

	/** 改变角色部件
		@param sectionType 部件类型(头发，身体等), 请参考RoleSectionType
		@param color 肤色，或发色
		@param index 索引, 表示同一肤色下的的第几组模型表现, 或表示同一发色下的的第几组模型表现
	*/
	void changeRoleSection( uint sectionType, uint color, uint index );
	void changeRoleSection( uint sectionType, uint modelDisplayID );
	void swap( Avatar & r );

	bool isHoldSlot(uint slot, EquipSlotType slotType);

public:
	ModelGcs * getModel();
	const RoleRaceRow * getRoleRaceInfo() const;

	/** 取得调整者
		返回值是有可能刷新的，所以返回的指针不能长久保存
	*/
	AvatarAdjuster * getAdjuster();
protected:
	void addShareModel( uint slot, uint id );
	void addHangModel( uint slot, uint modelDisplayID, const std::string & boneName );

	void setCurrHair(uint hair) { mCurrHair = hair; }
private:
	ModelGcs *					mModel;
	const RoleRaceRow *			mRoleRaceInfo;			// 角色类型信息

	ShareEntitys				mShareEntitys;			// 共享实体表
	HangEntitys					mHangEntitys;			// 挂接实体表
	ExtraHoldSlot				mExtraHoldSlot;			// 额外占据格(一般套装才有)

	uint						mBaseHair;				// 原始发型
	uint						mRepHair;				// 替换发型
	uint						mCurrHair;				// 当前发型
	uint						mHairRepType;			// 发型替换规则

	RoleSectionMap				mRoleSections;			// 角色部件表
	AvatarAdjuster *			mAvatarAdjuster;		// 角色部件调整

	uint						mSlotVisible;			// 装备栏可见
};



#endif
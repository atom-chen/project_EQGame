/******************************************************************************
* ����: Hongjx
* ʱ��: 2009-11-27
* ����: ��װ��ɫ��, ��װ�߼���
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


/** ��װ��ɫ��
	˵��: ÿ����ɫ���б�֤��mModel, mRoleRaceInfo��Ϊ��
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
	/** ��ɫ��������
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

	/** ���캯��
		˵��: ÿ����ɫ���б�֤��mModel, mRoleRaceInfo��Ϊ��
	*/
	explicit Avatar( uint id );
public:
	/** װ����ĳ����Ʒ
		@param solt װ����, ��ο�EquipSlotType
	*/
	void equipOn( uint slot, uint equipDisplayID, uint holdSlot );

	/** ж��ĳ��װ������������Ʒ
		@param solt װ����, ��ο�EquipSlotType
	*/
	void takeOff( uint slot );
	void takeOffAll();

	// Ϊ0ʱ��ԭʼ�����滻
	void replaceHair(uint repHair);

	/** ���ؽ�ɫ����
	*/
	void hideSlot( uint slotArr );
	void restoreSlot();

	/** �ı��ɫ����
		@param sectionType ��������(ͷ���������), ��ο�RoleSectionType
		@param color ��ɫ����ɫ
		@param index ����, ��ʾͬһ��ɫ�µĵĵڼ���ģ�ͱ���, ���ʾͬһ��ɫ�µĵĵڼ���ģ�ͱ���
	*/
	void changeRoleSection( uint sectionType, uint color, uint index );
	void changeRoleSection( uint sectionType, uint modelDisplayID );
	void swap( Avatar & r );

	bool isHoldSlot(uint slot, EquipSlotType slotType);

public:
	ModelGcs * getModel();
	const RoleRaceRow * getRoleRaceInfo() const;

	/** ȡ�õ�����
		����ֵ���п���ˢ�µģ����Է��ص�ָ�벻�ܳ��ñ���
	*/
	AvatarAdjuster * getAdjuster();
protected:
	void addShareModel( uint slot, uint id );
	void addHangModel( uint slot, uint modelDisplayID, const std::string & boneName );

	void setCurrHair(uint hair) { mCurrHair = hair; }
private:
	ModelGcs *					mModel;
	const RoleRaceRow *			mRoleRaceInfo;			// ��ɫ������Ϣ

	ShareEntitys				mShareEntitys;			// ����ʵ���
	HangEntitys					mHangEntitys;			// �ҽ�ʵ���
	ExtraHoldSlot				mExtraHoldSlot;			// ����ռ�ݸ�(һ����װ����)

	uint						mBaseHair;				// ԭʼ����
	uint						mRepHair;				// �滻����
	uint						mCurrHair;				// ��ǰ����
	uint						mHairRepType;			// �����滻����

	RoleSectionMap				mRoleSections;			// ��ɫ������
	AvatarAdjuster *			mAvatarAdjuster;		// ��ɫ��������

	uint						mSlotVisible;			// װ�����ɼ�
};



#endif
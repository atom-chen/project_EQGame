/******************************************************************************
* ����: Y3
* ʱ��: 2010-03-02
* ����: ����ƹ�
******************************************************************************/
#ifndef ZoneLight_H
#define ZoneLight_H

#include "EQMainPrerequisites.h"

#include "GraphicsTable.h"

/** ����ƹ�
@remarks ��Ҫ�Լ�����, ���ZoneLightMgr����
*/
class ZoneLight
{
public:
	/// ����ƹ�����
	enum ZoneLightType {
		ZLT_NONE,
		ZLT_ACTIVE,			//������
		ZLT_PASS,			//��͸��
	};

	/// �����⴩͸����, ���ܲ���������
	enum PierceType {
		PIERCE_NONE = 0,	//���ɴ�͸
		PIERCE_OUTDOOR = 1,	//�ɴ�͸������
		PIERCE_INDOOR = 2,	//�ɴ�͸������
	};

public:
	//�ӱ���ȡ�ƹ�

	//������Ҫ����������
	Ogre::Light* getLight() { return mLight; }

	const std::string& getName() const { return mName; }

	ZoneLightType getZoneLightType() { return mZoneLightType; }

	void setZoneLightType(ZoneLightType type) { mZoneLightType = type; }

	const std::string& getDesName() const { return mDesName; }

	void setDesName(const std::string& name) { mDesName = name; }

	/// ��ȡ��͸����. ֻ�����������������
	PierceType getPierceType() { return mPierceType; }
	void setPierceType(PierceType type) { mPierceType = type; }

	/// ��ȡ��͸��Χ
	uint getPierceRange() { return mPierceRange; }
	void setPierceRange(uint range) { mPierceRange = range; }

	/// ��ȡ��͸��Ӱ�����ڻ�������
	bool getPassLightIndoor() { return mIndoor; }
	void setPassLightIndoor(bool indoor) { mIndoor = indoor; }

	//pass��ʼ��˥��ϵ��
	//pass��ʼ��λ��

	/// �ƹ�Ӱ��������Ѿ�����Ⱦ��Ҫ���µƹ���Ӱ�������
	bool  isDirty(DWORD dirty, bool needUpdate);

	/// ֪ͨ��������ı�, ���������Ҫ�ֶ�����
	void notifyLightZoneChange(int id) { mLightZoneID = id; }

	int getLightZoneID() { return mLightZoneID; }

	const Ogre::Vector3& getPos() { return mPos; }
	void setPos(Ogre::Vector3& pos);

	float getAttenuation0(bool daynight);
	float getAttenuation1(bool daynight);
	float getAttenuation2(bool daynight);
	float getAttenuation3(bool daynight);
	void setAttenuation(bool daynight, float atten0, float atten1, float atten2, float atten3);

	void setNode(Ogre::SceneNode* node);
	Ogre::SceneNode* getNode() { return mNode; }

	void setPierceZoneID(int zone) { mPierceZoneID = zone; }
	int getPierceZoneID() { return mPierceZoneID; }

	void setForwardIndoor(bool indoor) { mForwardIndoor = indoor; }

public:
	ZoneLight(const std::string& name, ZoneLightType type=ZLT_NONE);
	~ZoneLight();

	void createLight();
	void loadActiveLight(const ActiveLightRow* r, bool visible);
	void loadPassLight(const PassLightRow* r);

	void turnOn();
	void turnOff();
	bool isOn();
	void turnOnPassLight(Ogre::ColourValue& color, float atten, float range);
	void setMute(bool m);
	bool getMute() {return mIsMute;}

	void smoothPassAttenChange(float per);

protected:
	void turn(bool t);

protected:
	// ��������
	Ogre::Light*		mLight;
	std::string			mName;
	Ogre::Vector3		mPos;
	int					mLightZoneID; /// ���ڵ�light zone id
	ZoneLightType		mZoneLightType;
	std::string			mDesName;/// ������������ƹ������
	Ogre::SceneNode*	mNode;
	bool				mIsOn;			/// �Ƿ���
	bool				mIsMute;		/// �Ƿ����Ƶ�, ���Ǳ�����

	// ������ר�� 
	PierceType			mPierceType;
	uint				mPierceRange;
	std::string			mBbsTexture;
	std::vector<Ogre::Vector3>		mBbsPos;
	Ogre::BillboardSet* mBbs;
	Ogre::BillboardType mBbsType;
	std::vector<Ogre::Billboard*>	mBb;

	Ogre::Billboard*	mBb1;
	Ogre::Vector3		mBbsPos2;
	Ogre::BillboardSet* mBbs2;
	Ogre::Billboard*	mBb2;

	// ��͸��ר��
	bool				mIndoor;/// ֻӰ�����ڻ�������
	float				mDayAtten[4];
	float				mNightAtten[4];
	float				mAttenFactor;
	int					mPierceZoneID; /// ��͸��Դzone
	DWORD				mDirty;
	bool				mForwardIndoor;/// �����������indoorֵ
};

#endif

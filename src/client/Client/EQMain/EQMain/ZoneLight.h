/******************************************************************************
* 作者: Y3
* 时间: 2010-03-02
* 描述: 区域灯光
******************************************************************************/
#ifndef ZoneLight_H
#define ZoneLight_H

#include "EQMainPrerequisites.h"

#include "GraphicsTable.h"

/** 区域灯光
@remarks 不要自己创建, 请从ZoneLightMgr创建
*/
class ZoneLight
{
public:
	/// 区域灯光类型
	enum ZoneLightType {
		ZLT_NONE,
		ZLT_ACTIVE,			//主动光
		ZLT_PASS,			//穿透光
	};

	/// 主动光穿透类型, 可能不放在这里
	enum PierceType {
		PIERCE_NONE = 0,	//不可穿透
		PIERCE_OUTDOOR = 1,	//可穿透到室外
		PIERCE_INDOOR = 2,	//可穿透到室内
	};

public:
	//从表格获取灯光

	//可能需要带创建功能
	Ogre::Light* getLight() { return mLight; }

	const std::string& getName() const { return mName; }

	ZoneLightType getZoneLightType() { return mZoneLightType; }

	void setZoneLightType(ZoneLightType type) { mZoneLightType = type; }

	const std::string& getDesName() const { return mDesName; }

	void setDesName(const std::string& name) { mDesName = name; }

	/// 获取穿透类型. 只有主动光才有这属性
	PierceType getPierceType() { return mPierceType; }
	void setPierceType(PierceType type) { mPierceType = type; }

	/// 获取穿透范围
	uint getPierceRange() { return mPierceRange; }
	void setPierceRange(uint range) { mPierceRange = range; }

	/// 获取穿透光影响室内或者室外
	bool getPassLightIndoor() { return mIndoor; }
	void setPassLightIndoor(bool indoor) { mIndoor = indoor; }

	//pass初始化衰减系数
	//pass初始化位置

	/// 灯光影响的区域已经被污染需要更新灯光所影响的区域
	bool  isDirty(DWORD dirty, bool needUpdate);

	/// 通知房间区域改变, 这个函数不要手动调用
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
	// 公用数据
	Ogre::Light*		mLight;
	std::string			mName;
	Ogre::Vector3		mPos;
	int					mLightZoneID; /// 所在的light zone id
	ZoneLightType		mZoneLightType;
	std::string			mDesName;/// 用于描述这个灯光的名称
	Ogre::SceneNode*	mNode;
	bool				mIsOn;			/// 是否开着
	bool				mIsMute;		/// 是否是哑的, 就是被屏蔽

	// 主动光专用 
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

	// 穿透光专用
	bool				mIndoor;/// 只影响室内或者室外
	float				mDayAtten[4];
	float				mNightAtten[4];
	float				mAttenFactor;
	int					mPierceZoneID; /// 穿透来源zone
	DWORD				mDirty;
	bool				mForwardIndoor;/// 窗户正方向的indoor值
};

#endif

/******************************************************************************
* 作者: Y3
* 时间: 2010-03-02
* 描述: 区域灯光
******************************************************************************/
#ifndef ZoneLightMgr_H
#define ZoneLightMgr_H

#include "EQMainPrerequisites.h"

#include "ZoneLight.h"

class ZoneLightMgr : public Ogre::Singleton<ZoneLightMgr>
{
public:
	typedef std::map<std::string, ZoneLight*> ZoneLightList;

	struct Zone
	{
		int						zoneNum;		//区域编号. 计算方式: 室外为0. 室内为 楼层数*100 + 房间编号
		ZoneLightList			zoneLight;		//区域光源
		ZoneLightList			pierceLight;	//区域具备穿透属性的光源
		DWORD					dirty;			//更新计数器
	};

public:
	ZoneLightMgr();
	~ZoneLightMgr();

	/// 创建区域灯光
	ZoneLight* createZoneLight(ZoneLight::ZoneLightType type, const std::string& name="");

	/// 创建区域灯光集
	std::vector<ZoneLight*> createZoneLights(const LightRow* l, bool createPass);

	/// 销毁区域灯光
	void destroyZoneLight(const std::string& name);
	void destroyZoneLight(ZoneLight* light);

	//更新需要修改了灯光, 需要更新环境
	void changeLightToZone(int newZoneNum, ZoneLight* l);

	DWORD getZoneDirty(int zoneNum);

	int getZoneIDByLight(Ogre::Light* l);

	ZoneLight* getZoneLightByName(const std::string& name);

	// 更新房间计数器
	void _notifyZoneLightsDirty(int zoneNum);

	void smoothDayNightChange(float per);
protected:
	void putLightInZone(int zoneNum, ZoneLight* l);
	void removeLightFromZone(ZoneLight* l);

	// 计算穿透光
	void _refreshPassLight(int zoneNum);
	void _calcPassLight(ZoneLight* passLight);

protected:
	ZoneLightList		mZoneLightList;

	std::map<int, Zone>	mZoneList;

	ulong				mAutoLightNum;

public:
	void turnOnAllLights();
	void turnOffAllLights();
};

//房间号为0一定为室外，房间号不为0不一定为室内，即存在属于室外的房间，比如阳台
extern inline uint room2zone(bool indoor, uint floor, uint room) 
{ 
	return (indoor && room!=0) ? floor*100+room : 0; 
}

extern inline bool zone2room(uint zone, uint& floor, uint& room) 
{ 
	floor = zone/100; 
	room = zone%100; 
	return zone==0; 
}

#endif

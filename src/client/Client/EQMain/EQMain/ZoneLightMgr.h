/******************************************************************************
* ����: Y3
* ʱ��: 2010-03-02
* ����: ����ƹ�
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
		int						zoneNum;		//������. ���㷽ʽ: ����Ϊ0. ����Ϊ ¥����*100 + ������
		ZoneLightList			zoneLight;		//�����Դ
		ZoneLightList			pierceLight;	//����߱���͸���ԵĹ�Դ
		DWORD					dirty;			//���¼�����
	};

public:
	ZoneLightMgr();
	~ZoneLightMgr();

	/// ��������ƹ�
	ZoneLight* createZoneLight(ZoneLight::ZoneLightType type, const std::string& name="");

	/// ��������ƹ⼯
	std::vector<ZoneLight*> createZoneLights(const LightRow* l, bool createPass);

	/// ��������ƹ�
	void destroyZoneLight(const std::string& name);
	void destroyZoneLight(ZoneLight* light);

	//������Ҫ�޸��˵ƹ�, ��Ҫ���»���
	void changeLightToZone(int newZoneNum, ZoneLight* l);

	DWORD getZoneDirty(int zoneNum);

	int getZoneIDByLight(Ogre::Light* l);

	ZoneLight* getZoneLightByName(const std::string& name);

	// ���·��������
	void _notifyZoneLightsDirty(int zoneNum);

	void smoothDayNightChange(float per);
protected:
	void putLightInZone(int zoneNum, ZoneLight* l);
	void removeLightFromZone(ZoneLight* l);

	// ���㴩͸��
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

//�����Ϊ0һ��Ϊ���⣬����Ų�Ϊ0��һ��Ϊ���ڣ���������������ķ��䣬������̨
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

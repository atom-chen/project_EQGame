#include "EQMainPCH.h"

#include "ZoneLightMgr.h"

//-----------------------------------------------------------------------
template<> ZoneLightMgr* Ogre::Singleton<ZoneLightMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
ZoneLightMgr::ZoneLightMgr() :
mAutoLightNum(0)
{
}
//-----------------------------------------------------------------------
ZoneLightMgr::~ZoneLightMgr()
{
	mZoneList.clear();

	for ( ZoneLightList::iterator i = mZoneLightList.begin();
		i != mZoneLightList.end(); ++i )
	{
		delete i->second;
	}
	mZoneLightList.clear();
}
//-----------------------------------------------------------------------
ZoneLight* ZoneLightMgr::createZoneLight(ZoneLight::ZoneLightType type, const std::string& name)
{
	std::string newName;

	if ( name == "" )
	{
		newName = "AutoName-ZoneLight-" + Ogre::StringConverter::toString(mAutoLightNum++);
		assert( mZoneLightList.find(newName) == mZoneLightList.end() );
	}
	else
	{
		if ( mZoneLightList.find(name) != mZoneLightList.end() )
			EQ_EXCEPT( "Light with name '" + name + "' already exists.", "ZoneLightMgr::createZoneLight" );

		newName = name;
	}

	ZoneLight* light = new ZoneLight(newName, type);

	mZoneLightList.insert(ZoneLightList::value_type(newName,light));

	putLightInZone(0, light);

	return light;
}
//-----------------------------------------------------------------------
std::vector<ZoneLight*> ZoneLightMgr::createZoneLights(const LightRow* l, bool createPass)
{
	std::vector<ZoneLight*> ll;

	if ( l->Light1 != NULL )
	{
		ZoneLight* zl = createZoneLight(ZoneLight::ZLT_ACTIVE);
		zl->loadActiveLight(l->Light1, l->Visible);
		ll.push_back(zl);
	}

	if ( l->Light2 != NULL )
	{
		ZoneLight* zl = createZoneLight(ZoneLight::ZLT_ACTIVE);
		zl->loadActiveLight(l->Light2, l->Visible);
		ll.push_back(zl);
	}

	if(createPass)
	{
		if (l->PassLi1!=NULL)
		{
			ZoneLight* zl = createZoneLight(ZoneLight::ZLT_PASS);
			zl->loadPassLight(l->PassLi1);
			ll.push_back(zl);
		}

		if (l->PassLi2!=NULL)
		{
			ZoneLight* zl = createZoneLight(ZoneLight::ZLT_PASS);
			zl->loadPassLight(l->PassLi2);
			ll.push_back(zl);
		}
	}

	return ll;
}
//-----------------------------------------------------------------------
void ZoneLightMgr::destroyZoneLight(const std::string& name)
{
	ZoneLightList::iterator it = mZoneLightList.find(name);
	if ( it == mZoneLightList.end() )
		return;

	removeLightFromZone(it->second);

	_notifyZoneLightsDirty(it->second->getLightZoneID());

	delete it->second;
	mZoneLightList.erase(it);
}
//-----------------------------------------------------------------------
void ZoneLightMgr::destroyZoneLight(ZoneLight* light)
{
	destroyZoneLight(light->getName());
}
//-----------------------------------------------------------------------
void ZoneLightMgr::changeLightToZone(int newZoneNum, ZoneLight* l)
{
	int oldZoneNum = l->getLightZoneID();

	removeLightFromZone(l);
	putLightInZone(newZoneNum, l);

	_notifyZoneLightsDirty(oldZoneNum);
	_notifyZoneLightsDirty(newZoneNum);
}
//-----------------------------------------------------------------------
void ZoneLightMgr::putLightInZone(int zoneNum, ZoneLight* l)
{
	std::map<int, Zone>::iterator it = mZoneList.find(zoneNum);
	Zone* zone;
	if ( it == mZoneList.end() )
	{
		Zone z;
		z.zoneNum = zoneNum;
		z.dirty= 0;

		zone = &mZoneList.insert(std::map<int, Zone>::value_type(zoneNum, z)).first->second;
	}
	else
		zone = &it->second;

	ZoneLightList::iterator itl = zone->zoneLight.find(l->getName());
	if ( itl != zone->zoneLight.end() )
		EQ_EXCEPT( "Light '" + l->getName() + "' exist in zone '" + Ogre::StringConverter::toString(zoneNum) + "'", "ZoneLightMgr::putLightInZone" );

	zone->zoneLight.insert(ZoneLightList::value_type(l->getName(), l));

	if ( (l->getPierceType() == ZoneLight::PIERCE_INDOOR && zoneNum == 0)			//室外匹配
		|| (l->getPierceType() == ZoneLight::PIERCE_OUTDOOR && zoneNum != 0) )		//室内匹配
	{
		ZoneLightList::iterator itp = zone->pierceLight.find(l->getName());
		if ( itp != zone->pierceLight.end() )
			EQ_EXCEPT( "Light pierce '" + l->getName() + "' exist in zone '" + Ogre::StringConverter::toString(zoneNum) + "'", "ZoneLightMgr::putLightInZone" );

		zone->pierceLight.insert(ZoneLightList::value_type(l->getName(), l));
	}

	//
	l->notifyLightZoneChange(zoneNum);
}
//-----------------------------------------------------------------------
void ZoneLightMgr::removeLightFromZone(ZoneLight* l)
{
	int id = l->getLightZoneID();

	std::map<int, Zone>::iterator it = mZoneList.find(id);
	if ( it == mZoneList.end() )
		return;

	Zone& zone = it->second;
	zone.zoneLight.erase(l->getName());
	zone.pierceLight.erase(l->getName());
}
//-----------------------------------------------------------------------
void ZoneLightMgr::_notifyZoneLightsDirty(int zoneNum)
{
	std::map<int, Zone>::iterator it = mZoneList.find(zoneNum);
	if(it == mZoneList.end())
		return;

	it->second.dirty++;

	_refreshPassLight(zoneNum);
}
//-----------------------------------------------------------------------
DWORD ZoneLightMgr::getZoneDirty(int zoneNum)
{
	std::map<int, Zone>::iterator it = mZoneList.find(zoneNum);
	if(it == mZoneList.end())
		return 0;

	return it->second.dirty;
}
//-----------------------------------------------------------------------
int ZoneLightMgr::getZoneIDByLight(Ogre::Light* l)
{
	ZoneLightList::iterator it;
	for(it=mZoneLightList.begin(); it!=mZoneLightList.end(); it++)
	{
		if(it->second->getLight() == l)
			return it->second->getLightZoneID();
	}

	return 0;
}
//-----------------------------------------------------------------------
void ZoneLightMgr::_refreshPassLight(int zoneNum)
{
	ZoneLightList::iterator it;
	for(it=mZoneLightList.begin(); it!=mZoneLightList.end(); it++)
	{
		//跳过主动光
		if(it->second->getZoneLightType() != ZoneLight::ZLT_PASS)
			continue;

		//跳过不在此房间的穿透光或不受此房间光照影响的穿透光
		if(it->second->getLightZoneID()!=zoneNum/*移动窗户*/ && it->second->getPierceZoneID()!=zoneNum/*移动灯*/)
			continue;

		//查询dirty值是否改变
		if(it->second->isDirty(getZoneDirty(zoneNum), it->second->getPierceZoneID()==zoneNum))
			_calcPassLight(it->second);
	}
}
//-----------------------------------------------------------------------
void ZoneLightMgr::_calcPassLight(ZoneLight* passLight)
{
	//穿透来源房间灯光列表
	std::map<int, Zone>::iterator itz = mZoneList.find(passLight->getPierceZoneID());
	if(itz == mZoneList.end())
		return;
	ZoneLightMgr::ZoneLightList* zllist = &itz->second.zoneLight;

	//遍历该房间内所有灯光
	ZoneLightMgr::ZoneLightList::iterator it;
	Ogre::ColourValue passColor;
	float passAtten=0;
	float passRange=0;
	ZoneLight* pierceLight;
	for(it=zllist->begin(); it!=zllist->end(); it++)
	{
		ZoneLight* light = it->second;

		//过滤非主动光
		if(light->getZoneLightType()!=ZoneLight::ZLT_ACTIVE) 
			continue;

		//方向光肯定衰减系数最大，就不计算其他的了
		if(light->getLight()->getType() == Ogre::Light::LT_DIRECTIONAL)
		{
			passColor = light->getLight()->getDiffuseColour();
			passAtten = 1.0f;
			passRange = 10000.0f;
			break;
		}

		//过滤不能产生穿透的光
		if(light->getPierceType()==ZoneLight::PIERCE_NONE || light->getPierceRange()==0)
			continue;

		//主动光还有分只能产生室外或室内穿透
		if((light->getPierceType()==ZoneLight::PIERCE_OUTDOOR && passLight->getPassLightIndoor()==true) || 
			(light->getPierceType()==ZoneLight::PIERCE_INDOOR && passLight->getPassLightIndoor()==false))
			continue;

		if(!passLight->getNode()->getParentSceneNode())
			continue;

		//取衰减系数最大的
		Ogre::Vector3 pos = passLight->getNode()->getParentSceneNode()->_getDerivedPosition();
		Ogre::Vector3 lpos = light->getLight()->getDerivedPosition();
		DWORD dis = (lpos - pos).length();
		float atten = 1 / (light->getLight()->getAttenuationConstant() + 
			light->getLight()->getAttenuationLinear()*dis + light->getLight()->getAttenuationQuadric()*dis*dis);

		//衰减太多的忽略掉
		if(atten < 0.1)
			continue;

		if(atten > passAtten)
		{
			passAtten = atten;
			passColor = light->getLight()->getDiffuseColour();
			passRange = light->getPierceRange();
			pierceLight = light;
		}
	}

	if(passAtten == 0)
		passLight->turnOff();
	else
		passLight->turnOnPassLight(passColor, passAtten, passRange);
}
//-----------------------------------------------------------------------
ZoneLight* ZoneLightMgr::getZoneLightByName(const std::string& name)
{
	ZoneLightList::iterator it = mZoneLightList.find(name);
	if ( it == mZoneLightList.end() )
		return NULL;

	return it->second;
}
//-----------------------------------------------------------------------
void ZoneLightMgr::turnOnAllLights()
{
	ZoneLightList::iterator it;
	for(it=mZoneLightList.begin(); it!=mZoneLightList.end(); it++)
	{
		if(it->second->getLight() && it->second->getLight()->getType()!=Ogre::Light::LT_DIRECTIONAL)
			it->second->turnOn();
	}
}
//-----------------------------------------------------------------------
void ZoneLightMgr::turnOffAllLights()
{
	ZoneLightList::iterator it;
	for(it=mZoneLightList.begin(); it!=mZoneLightList.end(); it++)
	{
		if(it->second->getLight() && it->second->getLight()->getType()!=Ogre::Light::LT_DIRECTIONAL)
			it->second->turnOff();
	}
}
//-----------------------------------------------------------------------
void ZoneLightMgr::smoothDayNightChange(float per)
{
	ZoneLightList::iterator it;
	for(it=mZoneLightList.begin(); it!=mZoneLightList.end(); it++)
	{
		if(it->second->getZoneLightType() != ZoneLight::ZLT_PASS)
			continue;

		it->second->smoothPassAttenChange(per);
	}
}
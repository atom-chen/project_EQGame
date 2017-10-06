#include "EQMainPCH.h"

#include "ZoneLight.h"
#include "SystemGcs.h"
#include "AmbientMgr.h"

ZoneLight::ZoneLight(const std::string& name, ZoneLightType type):
mLight(NULL),
mName(name),
mLightZoneID(-1),
mZoneLightType(type),
mNode(NULL),
mDirty(0),
mPierceType(PIERCE_NONE),
mBbs(NULL),
mPierceZoneID(-1),
mAttenFactor(1.001),
mIsOn(true),
mIsMute(false)
{
}

ZoneLight::~ZoneLight()
{
	if(mLight)
	{
		SystemGcs::getSingleton().getSceneManager()->destroyLight(mLight);

		if(mBbs)
		{
			for(uint i=0; i<mBb.size(); i++)
				mBbs->removeBillboard(mBb[i]);

			SystemGcs::getSingleton().getSceneManager()->destroyBillboardSet(mBbs);
		}
	}
}

void ZoneLight::createLight()
{
	assert(!mLight);

	mLight = SystemGcs::getSingleton().getSceneManager()->createLight(mName);
}

void ZoneLight::setPos(Ogre::Vector3& pos)
{
	mPos = pos;

	//主动灯可以立即更新，穿透光得另行计算
	if(mNode && mZoneLightType==ZLT_ACTIVE)
		mNode->setPosition(mPos);
}

void ZoneLight::setNode(Ogre::SceneNode* node)
{
	mNode = node;

 	if(mZoneLightType == ZLT_ACTIVE)
 	{
		mNode->attachObject(mLight);
		mNode->setPosition(mPos);

		if(!mBbsTexture.empty() && mBbsPos.size()>0)
		{
			if(!mBbs)
			{
				mBbs = SystemGcs::getSingleton().getSceneManager()->createBillboardSet(mName+"_bbs");
				mBbs->setBillboardType(mBbsType);
				//都是Y轴不转
				if(mBbsType == Ogre::BBT_ORIENTED_COMMON)
					mBbs->setCommonDirection(Ogre::Vector3::UNIT_Y);

				Ogre::SceneNode* n = mNode->getParentSceneNode()->createChildSceneNode();
				n->attachObject(mBbs);

				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("LightBbs");
				std::string newName;
				do {
					newName = mat->getName() + "-auto-" + Ogre::StringConverter::toString(Ogre::Math::UnitRandom());
				} while (Ogre::MaterialManager::getSingleton().resourceExists(newName));
				Ogre::MaterialPtr cloneMat = mat->clone(newName);

				mBbs->setMaterialName(newName);
				Ogre::Pass* pass = mBbs->getMaterial()->getTechnique(0)->getPass(0);
				pass->getTextureUnitState(0)->setTextureName(mBbsTexture);

				for(uint i=0; i<mBbsPos.size(); i++)
				{
					Ogre::Billboard* bb = mBbs->createBillboard(mBbsPos[i]);
					mBb.push_back(bb);
				}
			}
		}
	}
}

void ZoneLight::loadActiveLight(const ActiveLightRow* r, bool visible)
{
	/// 主动光load时就可以创建了
	mZoneLightType = ZLT_ACTIVE;
	mDesName = r->Name;
	mPierceType = (PierceType)r->Pierce;
	mPierceRange =  r->PierceRange;
	mPos = r->Pos;
	mBbsTexture = r->BbsTexture;
	mBbsPos = r->BbsPos;
	mBbsType = (Ogre::BillboardType)r->BbsType;

	if(!mLight)
		mLight = SystemGcs::getSingleton().getSceneManager()->createLight(mName);

	mLight->setType((Ogre::Light::LightTypes)r->LightType);
	mLight->setDiffuseColour(r->DiffCol);
	mLight->setSpecularColour(r->SpecCol);
	mLight->setAttenuation(r->Attenuation[0], r->Attenuation[1], r->Attenuation[2], r->Attenuation[3]);
	
	//聚光灯专有属性
	if(mLight->getType() == Ogre::Light::LT_SPOTLIGHT)
	{
		mLight->setSpotlightRange(Ogre::Degree(r->SpotRange[0]), Ogre::Degree(r->SpotRange[1]), r->SpotRange[2]);
		mLight->setDirection(r->Dir);
	}

	mIsOn = visible;
	mLight->setVisible(visible);
	if ( mIsMute )	//如果被屏蔽了, 就无效
		mLight->setVisible(false);
	mLight->setCastShadows(false);
}

void ZoneLight::loadPassLight(const PassLightRow* r)
{
	/// 穿透光不是立刻创建的，要做下缓冲
	mZoneLightType = ZLT_PASS;
	mDesName = r->Name;
	mIndoor = r->Indoor;

	memcpy(mDayAtten, r->DayAtten, 4*sizeof(float));
	memcpy(mNightAtten, r->NightAtten, 4*sizeof(float));
	mPos = r->Pos;
}

void ZoneLight::turn(bool t)
{
	if(!mLight)
		return;

	mLight->setVisible(t);

	if(mBbs)
		mBbs->setVisible(t);
}

void ZoneLight::turnOn()
{
	if(!mLight || mZoneLightType != ZLT_ACTIVE)
		return;

	mIsOn = true;

	if ( mIsMute )
		return;

	turn(true);
}

void ZoneLight::turnOff()
{
	if(!mLight || mZoneLightType != ZLT_ACTIVE)
		return;

	mIsOn = false;

	if ( mIsMute )
		return;

	turn(false);
}

bool ZoneLight::isOn()
{
	if(!mLight)
		return false;

	if ( mIsMute )
		return mIsOn;
	else
		return mLight->getVisible();
}

void ZoneLight::turnOnPassLight(Ogre::ColourValue& color, float atten, float range)
{
	if(mZoneLightType != ZLT_PASS)
		return;

	if(!mLight)
	{
		mLight = SystemGcs::getSingleton().getSceneManager()->createLight(mName);
		//穿透光只能是点光源
		mLight->setType(Ogre::Light::LT_POINT);
		mLight->setCastShadows(false);

		assert(mNode);
		mNode->attachObject(mLight);
	}

	assert(mLight);
	mLight->setDiffuseColour(color);
	mLight->setVisible(true);
	mIsOn = true;
	if (mIsMute)
	{
		//如果被屏蔽了, 就无效
		mLight->setVisible(false);
	}

	if(atten != mAttenFactor)
	{
		mAttenFactor = atten;
		if(AmbientMgr::getSingleton().getDayNight())
		{
			mLight->setAttenuation(range, mDayAtten[1]/mAttenFactor, mDayAtten[2]/mAttenFactor, mDayAtten[3]/mAttenFactor);
		}
		else
		{
			mLight->setAttenuation(range, mNightAtten[1]/mAttenFactor, mNightAtten[2]/mAttenFactor, mNightAtten[3]/mAttenFactor);
		}
	}

	//如果灯光indoor值和正方向格子indoor值一致，则取正向坐标，否则取反向坐标
	mNode->setPosition(mForwardIndoor==mIndoor? mPos : Ogre::Vector3(-mPos.x, mPos.y, mPos.z));
}

void ZoneLight::setMute(bool m)
{
	if ( m == mIsMute )
		return;

	mIsMute = m;

	bool t;
	if ( mIsMute )
		t = false;
	else
		t = mIsOn;

	turn(t);
}

bool ZoneLight::isDirty(DWORD dirty, bool needUpdate)
{
	if(mDirty == dirty)
		return false;

	if(needUpdate)
		mDirty = dirty;

	return true;
}

void ZoneLight::setAttenuation(bool daynight, float atten0, float atten1, float atten2, float atten3)
{
	if(daynight)
	{
		mDayAtten[0] = atten0;
		mDayAtten[1] = atten1;
		mDayAtten[2] = atten2;
		mDayAtten[3] = atten3;
	}
	else
	{
		mNightAtten[0] = atten0;
		mNightAtten[1] = atten1;
		mNightAtten[2] = atten2;
		mNightAtten[3] = atten3;
	}

	if(mLight)
		mLight->setAttenuation(mLight->getAttenuationRange(), atten1/mAttenFactor, atten2/mAttenFactor, atten3/mAttenFactor);
}

float ZoneLight::getAttenuation0(bool daynight)
{
	return daynight? mDayAtten[0] : mNightAtten[0];
}

float ZoneLight::getAttenuation1(bool daynight)
{
	return daynight? mDayAtten[1] : mNightAtten[1];
}

float ZoneLight::getAttenuation2(bool daynight)
{
	return daynight? mDayAtten[2] : mNightAtten[2];
}

float ZoneLight::getAttenuation3(bool daynight)
{
	return daynight? mDayAtten[3] : mNightAtten[3];
}

void ZoneLight::smoothPassAttenChange(float per)
{
	if(!mLight)
		return;

	if(mZoneLightType != ZLT_PASS)
		return;

	float atten1,atten2,atten3;
	if(AmbientMgr::getSingleton().getDayNight())
	{
		atten1 = mNightAtten[1] - per * (mNightAtten[1] - mDayAtten[1]);
		atten2 = mNightAtten[2] - per * (mNightAtten[2] - mDayAtten[2]);
		atten3 = mNightAtten[3] - per * (mNightAtten[3] - mDayAtten[3]);
	}
	else
	{
		atten1 = mDayAtten[1] - per * (mDayAtten[1] - mNightAtten[1]);
		atten2 = mDayAtten[2] - per * (mDayAtten[2] - mNightAtten[2]);
		atten3 = mDayAtten[3] - per * (mDayAtten[3] - mNightAtten[3]);
	}

	mLight->setAttenuation(mLight->getAttenuationRange(), atten1/mAttenFactor, atten2/mAttenFactor, atten3/mAttenFactor);
}
/******************************************************************************
* 作者: lf
* 时间: 2010-03-11
* 描述: 灯光监听
******************************************************************************/
#ifndef ZoneLightListner_H
#define ZoneLightListner_H

#include "EQMainPrerequisites.h"

#include "ZoneLightMgr.h"
#include "Ogre/OgreSceneNode.h"

class ZoneLightListener : public Ogre::MovableObject::Listener
{
public:
	ZoneLightListener(std::vector<uint>& zone):mZoneList(zone),mLightListUpdated(0),mNeedRefresh(false){}
	ZoneLightListener():mLightListUpdated(0),mNeedRefresh(false){}
	~ZoneLightListener(){};

private:
	std::vector<uint> mZoneList;
	Ogre::LightList mLightList;
	ulong mLightListUpdated;
	bool mNeedRefresh;

public:
	virtual const Ogre::LightList* objectQueryLights(const Ogre::MovableObject* mobject)
	{
		if (mobject->getParentNode())
		{
			Ogre::SceneNode* sn = static_cast<Ogre::SceneNode*>(mobject->getParentNode());

			// Make sure we only update this only if need.
			ulong frame = sn->getCreator()->_getLightsDirtyCounter();

			if(mZoneList.empty())
			{
				mLightList.clear();
			}
			else if (mLightListUpdated != frame || mNeedRefresh)
			{
				mLightListUpdated = frame;
				mNeedRefresh = false;

				Ogre::LightList templist;

				sn->findLights(templist, mobject->getBoundingRadius());

				// 引擎过滤完后再进行房间过滤
				mLightList.clear();
				mLightList.reserve(templist.size());
				for(int i=0; i<(int)templist.size(); i++)
				{
					if(std::find(mZoneList.begin(), mZoneList.end(), ZoneLightMgr::getSingleton().getZoneIDByLight(templist[i])) != mZoneList.end())
						mLightList.push_back(templist[i]);
				}
			}
		}
		else
		{
			mLightList.clear();
		}

		return &mLightList;
	}

	virtual void objectDestroyed(Ogre::MovableObject*)
	{
		delete this;
	}

	void setZoneList(std::vector<uint>& zone) 
	{
		mZoneList.clear();
		mZoneList = zone;
		mNeedRefresh = true;
	}
};

#endif
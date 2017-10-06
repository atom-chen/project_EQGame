/******************************************************************************
* 作者: lf
* 时间: 2010-9-3
* 描述: 场景基类
******************************************************************************/
#ifndef EQGameScene_H
#define EQGameScene_H

#include "MainPlayer.h"
#include "LogicTable.h"

class EQGameScene
{
public:
	EQGameScene(uint id):mSceneID(id),mListener(0){};
	virtual ~EQGameScene() {};

	uint getSceneID() { return mSceneID; }

protected:
	int mSceneID;
	vector<TeleportRow> mTeleports;

public:
	class Listener
	{
	public:
		virtual void postCreateScene(EQGameScene*) {};
		virtual void preDestroyScene(EQGameScene*) {};
	};

	void addListener(Listener* listener) { mListener = listener; }
	void removeListener() { delete mListener; mListener = NULL; }
	Listener* getListener() { return mListener; }

	void initTeleports()
	{
		const TeleportTable& table = LogicTableManager::getSingleton().getTeleportTable();
		
		stdext::hash_map<int, TeleportRow>::const_iterator it = table.begin();
		for (; it!=table.end();it++ )
		{
			if((*it).second.src_scene_id == mSceneID)
				mTeleports.push_back((*it).second);
		}
	}

	int checkTeleportID()
	{
		Ogre::Vector3 pos = sMainPlayer.getPos();
		vector<TeleportRow>::const_iterator it = mTeleports.begin();
		for (; it!=mTeleports.end();it++ )
		{
			Ogre::Vector3 tpos(Ogre::Vector3((*it).src_x,(*it).src_y,(*it).src_z));
			if (pos.distance(tpos) <= (*it).src_radius)
				return (*it).id;
		}
		return 0;
	}
private:
	Listener* mListener;
};

#endif
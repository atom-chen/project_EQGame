/******************************************************************************
* 作者: lf
* 时间: 2011-04-26
* 描述: 粒子系统
******************************************************************************/
#ifndef ParticleMgr_H
#define ParticleMgr_H

#include "EQMainPrerequisites.h"

namespace ParticleUniverse
{
	class ParticleSystem;
}

struct TransformInfo
{
	TransformInfo( Ogre::Vector3 pos = Ogre::Vector3::ZERO,
		Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY ) :
	mPosition(pos), mRotation(rotation)
	{
	}

	Ogre::Vector3 mPosition;
	Ogre::Quaternion mRotation;
};

enum eTransformType
{
	TRANS_NONE = 0,		//不同步	
	TRANS_POSITION,		//同步坐标
	TRANS_ORIENTATION,	//同步方向
	TRANS_POSORI,		//同步坐标和方向
};

enum eParticleQuality
{
	QUALITY_HIGH = 10,
	QUALITY_MEDIUM = 5,
	QUALITY_LOW = 1,
};

class Particle
{
public:
	Particle(uint id, Ogre::SceneManager* sm, Ogre::SceneNode* node);
	~Particle();

	void update();
	void create(const char* name, eParticleQuality quality);
	void destroy();
	bool isDestroy(){ return mDestroy; }

	void setVisible(bool visible);
	void setScale(Ogre::Vector3& scale);
	void setTransform(const TransformInfo& trans);
	uint getID() { return mID; }
	std::string& getBone() { return mBone; }
	eTransformType getTrans() { return mTrans; }

	void setQuality(eParticleQuality quality);

	bool isStartImmediately();
	void start();
	void pause();
	void stop();

protected:
	Ogre::SceneManager* mSceneMgr;
	ParticleUniverse::ParticleSystem*	mParticleSystem;
	Ogre::SceneNode*	mNode;

	uint				mID;
	std::string			mTemplate;
	uint				mLife; //0为不自动删除
	int					mDelay; //0为立即播放，-1为手动播放，>0为延迟播放
	Ogre::Vector3		mPos;
	Ogre::Quaternion	mRot;
	Ogre::Vector3		mScale;
	std::string			mBone;
	eTransformType		mTrans;

	bool				mVisible;
	bool				mStart;
	uint				mBegin;
	bool				mDestroy;
};

class ParticleMgr : public Ogre::Singleton<ParticleMgr>
{
public:
	ParticleMgr();
	~ParticleMgr();

	void init(Ogre::SceneManager* sm);
	void update();

	Particle* createParticle(uint id, const char* name="");
	void removeParticle(Particle* p);
	bool isAlive(Particle* p);

	void setQuality(eParticleQuality quality);

private:
	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode* mSceneNode;
	uint mCounter;
	std::map<Particle*, bool> mParticleMap;
	eParticleQuality mParticleQuality;
};

#endif
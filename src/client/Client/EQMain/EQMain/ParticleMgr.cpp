#include "EQMainPCH.h"

#include "ParticleMgr.h"
#include "CommFunc.h"
#include "GraphicsTable.h"

#pragma push_macro("new")
#undef new
#include "ParticleUniverse/ParticleUniverseSystemManager.h"
#pragma pop_macro("new")

//-----------------------------------------------------------------------
template<> ParticleMgr* Ogre::Singleton<ParticleMgr>::ms_Singleton = 0;

//-----------------------------------------------------------------------
Particle::Particle(uint id, Ogre::SceneManager* sm, Ogre::SceneNode* node):
mID(id),
mParticleSystem(NULL),
mVisible(true),
mDestroy(false),
mLife(0),
mStart(false)
{
	const ParticleRow* row = GraphicsTableManager::getSingleton().getParticleRow(id);
	if(!row)
		EQ_EXCEPT( FormatStr("Particle ID %d don't exist!", id), "Particle::Particle" );
	
	mTemplate = row->templateName;
	mLife = row->life;
	mDelay = row->delay;
	mPos = row->pos;
	mRot = row->rot;
	mScale = row->scale;
	mBone = row->bone;
	mTrans = (eTransformType)row->trans;

	mNode = node;
	mSceneMgr = sm;
}
//-----------------------------------------------------------------------
Particle::~Particle()
{
	
}
//-----------------------------------------------------------------------
void Particle::create(const char* name, eParticleQuality quality)
{
	ParticleUniverse::ParticleSystemManager* pManager = ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	mParticleSystem = pManager->createParticleSystem(name, mTemplate, mSceneMgr);

	mNode->attachObject(mParticleSystem);
	mParticleSystem->setScale(mScale);
	mParticleSystem->setCastShadows(false);
	mDestroy = false;

	if(quality != QUALITY_HIGH)
	{
		float scale = float(quality) / QUALITY_HIGH;

		for(uint i=0; i<mParticleSystem->getNumTechniques(); i++)
		{
			ParticleUniverse::ParticleTechnique* tech = mParticleSystem->getTechnique(i);
			uint quota = tech->getVisualParticleQuota();
			quota *= scale;
			if(quota <= 0) quota = 1;
			tech->setVisualParticleQuota(quota);
		}
	}

	if(mDelay < 0)
		mStart = false;
	else
		mStart = true;

	mParticleSystem->prepare();
	mBegin = GetTickCount();
}
//-----------------------------------------------------------------------
void Particle::destroy()
{
	if(mDestroy)
		return;

	ParticleUniverse::ParticleSystemManager* pManager = ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	pManager->destroyParticleSystem(mParticleSystem, mSceneMgr);
	
	mParticleSystem = NULL;
	mStart = false;
	mDestroy = true;
}
//-----------------------------------------------------------------------
bool Particle::isStartImmediately()
{
	return mDelay == 0;
}
//-----------------------------------------------------------------------
void Particle::start()
{
	if(mParticleSystem)
	{
		mParticleSystem->start();

		if(!mStart)
		{
			mStart = true;
			mBegin = GetTickCount();
		}
	}
}
//-----------------------------------------------------------------------
void Particle::pause()
{
	if(mParticleSystem)
		mParticleSystem->pause();
}
//-----------------------------------------------------------------------
void Particle::stop()
{
	if(mParticleSystem)
		mParticleSystem->stop();
}
//-----------------------------------------------------------------------
void Particle::update()
{
	if(!mStart)
		return;

	DWORD cur = GetTickCount();

	if(mDelay > 0)
	{
		if(cur - mBegin <= uint(mDelay))
			return;
		else
		{
			mBegin += mDelay;
			mParticleSystem->start();
		}
	}

	if(mLife>0 && cur>mBegin+mLife)
		destroy();
}
//-----------------------------------------------------------------------
void Particle::setVisible(bool visible)
{
	if(mParticleSystem && mNode)
	{
		if(visible)
		{
			if (!mParticleSystem->getParentNode())
				mNode->attachObject(mParticleSystem);
		}
		else
		{
			if (mParticleSystem->getParentNode())
				mParticleSystem->getParentSceneNode()->detachObject(mParticleSystem);
		}

		mVisible = visible;
		mNode->setVisible(mVisible);
	}
}
//-----------------------------------------------------------------------
void Particle::setScale(Ogre::Vector3& scale)
{
	if(mParticleSystem)
	{
		mParticleSystem->setScale(scale*mScale);
	}
}
//-----------------------------------------------------------------------
void Particle::setTransform(const TransformInfo& trans)
{
	if(mNode)
	{
		mNode->setPosition(trans.mPosition + trans.mRotation*mPos);
		mNode->setOrientation(trans.mRotation*mRot);
	}
}
//-----------------------------------------------------------------------
void Particle::setQuality(eParticleQuality quality)
{
	if(mParticleSystem)
	{
		std::string name = mParticleSystem->getName();
		bool started = mStart;

		destroy();
		create(name.c_str(), quality);
		if(started)
			start();
	}
}
//-----------------------------------------------------------------------






//-----------------------------------------------------------------------
ParticleMgr::ParticleMgr():
mCounter(0),
mParticleQuality(QUALITY_HIGH)
{
}
//-----------------------------------------------------------------------
ParticleMgr::~ParticleMgr()
{

}
//-----------------------------------------------------------------------
void ParticleMgr::init(Ogre::SceneManager* sm)
{
	mSceneMgr = sm;
	mSceneNode = mSceneMgr->getRootSceneNode();
}
//-----------------------------------------------------------------------
Particle* ParticleMgr::createParticle(uint id, const char* name/* = */)
{
	mCounter++;
	std::string str = name;
	if(str.empty())
		str = FormatStr("EQParticle_%d", mCounter);

	Particle* p = new Particle(id, mSceneMgr, mSceneNode->createChildSceneNode());
	p->create(str.c_str(), mParticleQuality);
	mParticleMap.insert( std::make_pair(p, true) );

	return p;
}
//-----------------------------------------------------------------------
void ParticleMgr::removeParticle(Particle* p)
{
	std::map<Particle*, bool>::iterator it = mParticleMap.find(p);
	if(it != mParticleMap.end())
	{
		p->destroy();
		delete p;
		mParticleMap.erase(it);
	}
}
//-----------------------------------------------------------------------
bool ParticleMgr::isAlive(Particle* p)
{
	std::map<Particle*, bool>::iterator it = mParticleMap.find(p);
	if(it != mParticleMap.end())
		return it->second;

	return false;
}
//-----------------------------------------------------------------------
void ParticleMgr::update()
{
	std::map<Particle*, bool>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); it++)
	{
		if(it->second == false)
			continue;

		it->first->update();
		if(it->first->isDestroy())
			it->second = false;
	}
}
//-----------------------------------------------------------------------
void ParticleMgr::setQuality(eParticleQuality quality)
{
	if(quality == mParticleQuality)
		return;

	mParticleQuality = quality;

	std::map<Particle*, bool>::iterator it;
	for(it=mParticleMap.begin(); it!=mParticleMap.end(); it++)
		it->first->setQuality(quality);
}
//-----------------------------------------------------------------------
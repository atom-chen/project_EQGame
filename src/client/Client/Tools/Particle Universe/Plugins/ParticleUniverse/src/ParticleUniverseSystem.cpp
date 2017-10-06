/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "ParticleUniverseSystem.h"
#include "ParticleUniverseSystemManager.h"
#include "OgreControllerManager.h"
#include "OgreController.h"

namespace ParticleUniverse
{
	// Constants
	const bool ParticleSystem::DEFAULT_KEEP_LOCAL = false;
	const Ogre::Real ParticleSystem::DEFAULT_ITERATION_INTERVAL = 0.0f;
	const Ogre::Real ParticleSystem::DEFAULT_FIXED_TIMEOUT = 0.0f;
	const Ogre::Real ParticleSystem::DEFAULT_NON_VISIBLE_UPDATE_TIMEOUT = 0.0f;
	const bool ParticleSystem::DEFAULT_SMOOTH_LOD = false;
	const Ogre::Real ParticleSystem::DEFAULT_FAST_FORWARD_TIME = 0.0f;
	const Ogre::String ParticleSystem::DEFAULT_MAIN_CAMERA_NAME = Ogre::StringUtil::BLANK;
	const Ogre::Real ParticleSystem::DEFAULT_SCALE_VELOCITY = 1.0f;
	const Ogre::Real ParticleSystem::DEFAULT_SCALE_TIME = 1.0f;
	const Ogre::Vector3 ParticleSystem::DEFAULT_SCALE = Ogre::Vector3::UNIT_SCALE;
	const bool ParticleSystem::DEFAULT_TIGHT_BOUNDINGBOX = false;

	//-----------------------------------------------------------------------
	class ParticleSystemUpdateValue : public Ogre::ControllerValue<Ogre::Real>
	{
		protected:
			ParticleSystem* mTarget;
		
		public:
			ParticleSystemUpdateValue(ParticleSystem* target) : mTarget(target) {}
			
			Ogre::Real getValue(void) const {return 0;}
			void setValue(Ogre::Real value) {mTarget->_update(value);}
	};
	//-----------------------------------------------------------------------
	ParticleSystem::ParticleSystem(const Ogre::String& name) :
		IElement(),
		mAABB(),
		MovableObject(name),
		mSceneManager(0),
		mTimeController(0),
		mTimeSinceLastVisible(0.0f),
		mLastVisibleFrame(0),
		mState(ParticleSystem::PSS_STOPPED),
		mNonvisibleUpdateTimeout(DEFAULT_NON_VISIBLE_UPDATE_TIMEOUT),
		mNonvisibleUpdateTimeoutSet(false),
		mResourceGroupName(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME),
		mSmoothLod(DEFAULT_SMOOTH_LOD),
		mSuppressNotifyEmissionChange(true),
		mIterationInterval(DEFAULT_ITERATION_INTERVAL),
		mIterationIntervalSet(false),
		mTimeSinceLastUpdate(0.0f),
		mFixedTimeout(DEFAULT_FIXED_TIMEOUT),
		mFixedTimeoutSet(false),
		mBoundsAutoUpdate(true),
		mBoundsUpdateTime(10.0f),
		mOriginalBoundsUpdateTime(10.0f),
		mFastForwardSet(false),
		mOriginalFastForwardSet(false),
		mFastForwardTime(DEFAULT_FAST_FORWARD_TIME),
		mFastForwardInterval(0.0f),
		mMainCameraName(DEFAULT_MAIN_CAMERA_NAME),
		mMainCameraNameSet(false),
		mCurrentCamera(0),
		mParticleSystemScale(DEFAULT_SCALE),
		mParticleSystemScaleVelocity(DEFAULT_SCALE_VELOCITY),
		mParticleSystemScaleTime(DEFAULT_SCALE_TIME),
		mKeepLocal(DEFAULT_KEEP_LOCAL),
		mTightBoundingBox(DEFAULT_TIGHT_BOUNDINGBOX),
		mPauseTime(0.0f),
		mPauseTimeSet(false),
		mPauseTimeElapsed(0.0f),
		mTemplateName(Ogre::StringUtil::BLANK),
		mStopFadeSet(false),
		mLatestOrientation(Ogre::Quaternion::IDENTITY),
		mRotationOffset(Ogre::Quaternion::IDENTITY),
		mRotationCentre(Ogre::Vector3::ZERO)
	{
		mBoundingRadius = 1;
		particleType = PT_SYSTEM;
	}
	//-----------------------------------------------------------------------
	ParticleSystem::ParticleSystem(const Ogre::String& name, const Ogre::String& resourceGroupName) :
		IElement(),
		mAABB(),
		MovableObject(name),
		mSceneManager(0),
		mTimeController(0),
		mTimeSinceLastVisible(0.0f),
		mLastVisibleFrame(0),
		mState(ParticleSystem::PSS_STOPPED),
		mNonvisibleUpdateTimeout(DEFAULT_NON_VISIBLE_UPDATE_TIMEOUT),
		mNonvisibleUpdateTimeoutSet(false),
		mResourceGroupName(resourceGroupName),
		mSmoothLod(DEFAULT_SMOOTH_LOD),
		mSuppressNotifyEmissionChange(true),
		mIterationInterval(DEFAULT_ITERATION_INTERVAL),
		mIterationIntervalSet(false),
		mTimeSinceLastUpdate(0.0f),
		mFixedTimeout(DEFAULT_FIXED_TIMEOUT),
		mFixedTimeoutSet(false),
		mBoundsAutoUpdate(true),
		mBoundsUpdateTime(10.0f),
		mOriginalBoundsUpdateTime(10.0f),
		mFastForwardSet(false),
		mOriginalFastForwardSet(false),
		mFastForwardTime(DEFAULT_FAST_FORWARD_TIME),
		mFastForwardInterval(0.0f),
		mMainCameraName(DEFAULT_MAIN_CAMERA_NAME),
		mMainCameraNameSet(false),
		mCurrentCamera(0),
		mParticleSystemScale(DEFAULT_SCALE),
		mParticleSystemScaleVelocity(DEFAULT_SCALE_VELOCITY),
		mParticleSystemScaleTime(DEFAULT_SCALE_TIME),
		mKeepLocal(DEFAULT_KEEP_LOCAL),
		mTightBoundingBox(DEFAULT_TIGHT_BOUNDINGBOX),
		mPauseTime(0.0f),
		mPauseTimeSet(false),
		mPauseTimeElapsed(0.0f),
		mTemplateName(Ogre::StringUtil::BLANK),
		mStopFadeSet(false),
		mLatestOrientation(Ogre::Quaternion::IDENTITY),
		mRotationOffset(Ogre::Quaternion::IDENTITY),
		mRotationCentre(Ogre::Vector3::ZERO)
	{
		mBoundingRadius = 1;
	}
	//-----------------------------------------------------------------------
	ParticleSystem::~ParticleSystem(void)
	{
		// Destroy nodes that are used for renderers and stuff.
		if (getParentSceneNode())
		{
			getParentSceneNode()->removeAndDestroyAllChildren();
		}

		destroyAllTechniques();

		if (mTimeController)
		{
			// Destroy controller
			Ogre::ControllerManager::getSingleton().destroyController(mTimeController);
			mTimeController = 0;
		}
	}
	//-----------------------------------------------------------------------
	ParticleSystem& ParticleSystem::operator=(const ParticleSystem& ps)
	{
		// Destroy all existing techniques.
		destroyAllTechniques();

		// Copy techniques
		size_t i = 0;
		ParticleTechnique* psTechnique = 0;
		ParticleTechnique* clonedTechnique = 0;
		for(i = 0; i < ps.getNumTechniques(); ++i)
		{
			psTechnique = ps.getTechnique(i);
			clonedTechnique = ParticleSystemManager::getSingletonPtr()->cloneTechnique(psTechnique);
			addTechnique(clonedTechnique);
		}

		// Copy attributes
		mNonvisibleUpdateTimeout = ps.mNonvisibleUpdateTimeout;
		mNonvisibleUpdateTimeoutSet = ps.mNonvisibleUpdateTimeoutSet;
		mResourceGroupName = ps.mResourceGroupName;
		mLodDistanceList = ps.mLodDistanceList;
		mSmoothLod  = ps.mSmoothLod;
		mIterationIntervalSet = ps.mIterationIntervalSet;
		mIterationInterval = ps.mIterationInterval;
		mTimeSinceLastUpdate = ps.mTimeSinceLastUpdate;
		mFixedTimeout = ps.mFixedTimeout;
		mFixedTimeoutSet = ps.mFixedTimeoutSet;
		mState = ps.mState;
		mTimeElapsedSinceStart = ps.mTimeElapsedSinceStart;
		mSuppressNotifyEmissionChange = ps.mSuppressNotifyEmissionChange;
		mFastForwardSet = ps.mFastForwardSet;
		mOriginalFastForwardSet = ps.mOriginalFastForwardSet;
		mFastForwardTime = ps.mFastForwardTime;
		mFastForwardInterval = ps.mFastForwardInterval;
		mMainCameraName = ps.mMainCameraName;
		mMainCameraNameSet = ps.mMainCameraNameSet;
		mParticleSystemScale = ps.mParticleSystemScale;
		mParticleSystemScaleVelocity = ps.mParticleSystemScaleVelocity;
		mParticleSystemScaleTime = ps.mParticleSystemScaleTime;
		mKeepLocal = ps.mKeepLocal;
		mTightBoundingBox = ps.mTightBoundingBox;
		mPauseTime = ps.mPauseTime;
		mPauseTimeSet = ps.mPauseTimeSet;
		mPauseTimeElapsed = ps.mPauseTimeElapsed;
		mTemplateName = ps.mTemplateName;
		mSceneManager = ps.mSceneManager;
		mStopFadeSet = ps.mStopFadeSet;

		// Notify.
		mSuppressNotifyEmissionChange = false;
		_notifyEmissionChange();
		_notifyRescaled();
		_notifyVelocityRescaled();

		return *this;
    }
	//-----------------------------------------------------------------------
	void ParticleSystem::addParticleSystemListener (ParticleSystemListener* particleSystemListener)
	{
		mParticleSystemListenerList.push_back(particleSystemListener);
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::removeParticleSystemListener (ParticleSystemListener* particleSystemListener)
	{
		assert(particleSystemListener && "ParticleSystemListener is null!");
		ParticleSystemListenerIterator it;
		ParticleSystemListenerIterator itEnd = mParticleSystemListenerList.end();
		for (it = mParticleSystemListenerList.begin(); it != itEnd; ++it)
		{
			if (*it == particleSystemListener)
			{
				// Remove it (don´t destroy it, because the ParticleSystem is not the owner)
				mParticleSystemListenerList.erase(it);
				break;
			}
		}
	}
	//-----------------------------------------------------------------------
	bool ParticleSystem::hasTightBoundingBox(void) const
	{
		return mTightBoundingBox;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setTightBoundingBox(bool tightBoundingBox)
	{
		mTightBoundingBox = tightBoundingBox;
	}
	//-----------------------------------------------------------------------
	Ogre::Real ParticleSystem::getPauseTime(void) const
	{
		return mPauseTime;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setPauseTime(Ogre::Real pauseTime)
	{
		mPauseTime = pauseTime;
		mPauseTimeSet = true;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystem::getTemplateName(void) const
	{
		return mTemplateName;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setTemplateName(const Ogre::String& templateName)
	{
		mTemplateName = templateName;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setEnabled(bool enabled)
	{
		Particle::setEnabled(enabled);
		if (enabled && mState == !PSS_STARTED)
		{
			start();
		}
		else
		{
			stop();
		}
	}
	//-----------------------------------------------------------------------
	Ogre::SceneManager* ParticleSystem::getSceneManager(void)
	{
		return mSceneManager;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setSceneManager(Ogre::SceneManager* sceneManager)
	{
		mSceneManager = sceneManager;
	}
	//-----------------------------------------------------------------------
	bool ParticleSystem::isKeepLocal(void) const
	{
		return mKeepLocal;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setKeepLocal(bool keepLocal)
	{
		mKeepLocal = keepLocal;
	}
	//-----------------------------------------------------------------------
	bool ParticleSystem::makeParticleLocal(Particle* particle)
	{
		if (!particle)
			return true;

		if (!mKeepLocal || hasEventFlags(Particle::PEF_EXPIRED))
			return false;

		Ogre::Vector3 diff = getDerivedPosition() - latestPosition;
		particle->position += diff;
		return true;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& ParticleSystem::getDerivedPosition(void)
	{
		if (mMarkedForEmission)
		{
			// Use the position, because it is emitted
			mDerivedPosition = position;
		}
		else
		{
			if (mParentNode)
			{
				mDerivedPosition = mParentNode->_getDerivedPosition();
			}
			else
			{
				mDerivedPosition = Ogre::Vector3::ZERO;
			}
		}
		return mDerivedPosition;
	}
	//-----------------------------------------------------------------------
	const Ogre::Quaternion& ParticleSystem::getDerivedOrientation(void) const
	{
		if (mMarkedForEmission)
		{
			if (parentEmitter)
			{
				// Return the derived orientation of the uber ParticleSystem
				return parentEmitter->getParentTechnique()->getParentSystem()->getDerivedOrientation();
			}
			else
			{
				// Don´t know
				return Ogre::Quaternion::IDENTITY;
			}
		}
		else
		{
			if (mParentNode)
			{
				return mParentNode->_getDerivedOrientation();
			}
			else
			{
				return Ogre::Quaternion::IDENTITY;
			}
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Quaternion& ParticleSystem::getLatestOrientation(void) const
	{
		return mLatestOrientation;
	}
	//-----------------------------------------------------------------------
	bool ParticleSystem::hasRotatedBetweenUpdates(void) const
	{
		return mLatestOrientation != getDerivedOrientation();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::calulateRotationOffset(void)
	{
		if (mMarkedForEmission)
		{
			// Use the uber particle system as centre of rotation and not the particle systems' own position.
			mRotationCentre = parentEmitter->getParentTechnique()->getParentSystem()->getDerivedPosition();
		}
		else
		{
			// Use its own position
			mRotationCentre = getDerivedPosition();
		}

		/** Use the derived orientation, which is the particle systems' own scenenode orientation,
			or the orientation of the uber particle system, if this particle system is emitted itself.
		*/
		mRotationOffset = getDerivedOrientation() * mLatestOrientation.Inverse();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::rotationOffset(Ogre::Vector3& pos)
	{
		/** Calulate the rotated position.
		*/
		pos = mRotationCentre + mRotationOffset * (pos - mRotationCentre);
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystem::createTechnique(void)
	{
		ParticleTechnique* technique = ParticleSystemManager::getSingletonPtr()->createTechnique();
		addTechnique(technique);
		return technique;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::addTechnique(ParticleTechnique* technique)
	{
		mTechniques.push_back(technique);
		technique->setParentSystem(this);
		technique->_notifyRescaled(mParticleSystemScale);
		technique->_notifyVelocityRescaled(mParticleSystemScaleVelocity);
		_notifyEmissionChange();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::removeTechnique(ParticleTechnique* technique)
	{
		assert(technique && "Technique is null!");
		ParticleTechniqueIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			if ((*it) == technique)
			{
				mTechniques.erase(it);
				break;
			}
		}

		// It has no parent anymore
		technique->setParentSystem(0);

		// Notify, because the removed technique could have emitters that emit other techniques.
		_notifyEmissionChange();
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystem::getTechnique (size_t index) const
	{
		assert(index < mTechniques.size() && "Technique index out of bounds!");
		return mTechniques[index];
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystem::getTechnique (const Ogre::String& techniqueName) const
	{
		if (techniqueName == Ogre::StringUtil::BLANK)
			return 0;

		ParticleTechniqueList::const_iterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			if ((*it)->getName() == techniqueName)
			{
				return *it;
			}
		}

		return 0;
	}
	//-----------------------------------------------------------------------
	size_t ParticleSystem::getNumTechniques (void) const
	{
		return mTechniques.size();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::destroyTechnique(ParticleTechnique* technique)
	{
		assert(technique && "Technique is null!");
		ParticleTechniqueIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			if ((*it) == technique)
			{
				ParticleSystemManager::getSingletonPtr()->destroyTechnique(*it);
				mTechniques.erase(it);
				break;
			}
		}

		// Notify, because the destroyed technique could have emitters that emit other techniques.
		_notifyEmissionChange();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::destroyTechnique (size_t index)
	{
		destroyTechnique(getTechnique(index));
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::destroyAllTechniques (void)
	{
		ParticleTechniqueIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			ParticleSystemManager::getSingletonPtr()->destroyTechnique(*it);
		}
		mTechniques.clear();
	}
	//-----------------------------------------------------------------------
	const Ogre::Real ParticleSystem::getFastForwardTime(void) const
	{
		return mFastForwardTime;
	}
	//-----------------------------------------------------------------------
	const Ogre::Real ParticleSystem::getFastForwardInterval(void) const
	{
		return mFastForwardInterval;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setFastForward(Ogre::Real time, Ogre::Real interval)
	{
		mFastForwardSet = true;
		mOriginalFastForwardSet = true;
		mFastForwardTime = time;
		mFastForwardInterval = interval;
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystem::getMainCameraName(void) const
	{
		return mMainCameraName;
	}
	//-----------------------------------------------------------------------
	Ogre::Camera* ParticleSystem::getMainCamera(void)
	{
		if (mSceneManager)
		{
			return mSceneManager->getCamera(mMainCameraName);
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	bool ParticleSystem::hasMainCamera(void)
	{
		return mMainCameraNameSet;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setMainCameraName(Ogre::String cameraName)
	{
		mMainCameraName = cameraName;
		mMainCameraNameSet = true;
		if (mSceneManager)
		{
			mCurrentCamera = mSceneManager->getCamera(cameraName);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setMainCamera(Ogre::Camera* camera)
	{
		if (!camera)
			return;
		
		mCurrentCamera = camera;
		mMainCameraName = camera->getName();
		mMainCameraNameSet = true;
	}
	//-----------------------------------------------------------------------
	Ogre::Camera* ParticleSystem::getCurrentCamera(void)
	{
		return mCurrentCamera;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::fastForward(void)
	{
		if (!mFastForwardSet)
			return;

		for (Ogre::Real ftime = 0; ftime < mFastForwardTime; ftime += mFastForwardInterval)
		{
			_update(mFastForwardInterval);
		}
		
		mFastForwardSet = false;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_notifyAttached(Ogre::Node* parent, bool isTagPoint)
	{
		MovableObject::_notifyAttached(parent, isTagPoint);
		
		// Notify all techniques that the particle system is attached or detached
		ParticleTechniqueIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_notifyAttached(parent, isTagPoint);
		}

		if (parent)
		{
			// Create controller
			if (!mTimeController)
			{
				mTimeSinceLastVisible = 0;
				mLastVisibleFrame = Ogre::Root::getSingleton().getNextFrameNumber();
				Ogre::ControllerManager& controllerManager = Ogre::ControllerManager::getSingleton(); 
				Ogre::ControllerValueRealPtr particleSystemUpdateValue(OGRE_NEW ParticleSystemUpdateValue(this));
				mTimeController = controllerManager.createFrameTimePassthroughController(particleSystemUpdateValue);
			}
		}
		else if (!parent && mTimeController)
		{
			// Destroy controller
			Ogre::ControllerManager::getSingleton().destroyController(mTimeController);
			mTimeController = 0;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_notifyCurrentCamera(Ogre::Camera* cam)
	{
		mCurrentCamera = cam;
		Ogre::MovableObject::_notifyCurrentCamera(cam);
		mLastVisibleFrame = Ogre::Root::getSingleton().getNextFrameNumber();
		mTimeSinceLastVisible = 0.0f;

		// Notify all techniques
		ParticleTechniqueIterator it;
		Ogre::Vector3 vec = Ogre::Vector3::ZERO;
		Ogre::Real squareDistance = 0;
		Ogre::Vector3 vecCameraParentNode = cam->getDerivedPosition() - getParentNode()->_getDerivedPosition();
		Ogre::Real squareDistanceCameraParentNode = vecCameraParentNode.squaredLength();
		unsigned short index = 0;
		bool doCamera = !mMainCameraNameSet || (mMainCameraNameSet && mMainCameraName == cam->getName());

		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			// Calculate the distance between the camera and each ParticleTechnique (although it isn´t always used).
			if ((*it)->_isMarkedForEmission())
			{
				vec = cam->getDerivedPosition() - (*it)->position;
				squareDistance = vec.squaredLength();
			}
			else
			{
				vec = vecCameraParentNode;
				squareDistance = squareDistanceCameraParentNode;
			}

			if (doCamera)
			{
				// Only set the square distance in certain conditions
				(*it)->setCameraSquareDistance(squareDistance);
			}

			// Determine lod, enable technique with the appropriate lod index 
			// (only if the distance list is filled). Note, that this really only works with a
			// 1-camera setup.
			if (!mLodDistanceList.empty())
			{
				if (doCamera)
				{
					index = _getLodIndexSquaredDistance(squareDistance);
					if ((*it)->getLodIndex() == index)
					{
						(*it)->setEnabled(true);
					}
					else
					{
						(*it)->setEnabled(false);
					}
				}
			}

			// Notify technique
			(*it)->_notifyCurrentCamera(cam);
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::String& ParticleSystem::getMovableType(void) const
	{
		return ParticleSystemFactory::PU_FACTORY_TYPE_NAME;
	}
	//-----------------------------------------------------------------------
	const Ogre::AxisAlignedBox& ParticleSystem::getBoundingBox(void) const
	{
		return mAABB;
	}
	//-----------------------------------------------------------------------
	Ogre::Real ParticleSystem::getBoundingRadius(void) const
	{
		return mBoundingRadius;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_updateRenderQueue(Ogre::RenderQueue* queue)
	{
		// Update renderqueues of all techniques
		ParticleTechniqueIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_updateRenderQueue(queue);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setRenderQueueGroup(Ogre::uint8 queueId)
	{
		MovableObject::setRenderQueueGroup(queueId);

		// Set the same group for all renderers in all techniques
		ParticleTechniqueIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->setRenderQueueGroup(queueId);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_update(Ogre::Real timeElapsed)
	{
		// Only update if attached to a node
		if (!mParentNode)
			return;

		// Only update if the particle system is started or prepare the particle system before starting.
		if (mState == ParticleSystem::PSS_STARTED)
		{
			if (mNonvisibleUpdateTimeoutSet)
			{
				long frameDiff = Ogre::Root::getSingleton().getNextFrameNumber() - mLastVisibleFrame;
				if (frameDiff > 1 || frameDiff < 0)
				{
					mTimeSinceLastVisible += timeElapsed;
					if (mTimeSinceLastVisible >= mNonvisibleUpdateTimeout)
					{
						// No update
						return;
					}
				}
			}

			// Speedup or slowdown
			timeElapsed *= mParticleSystemScaleTime;

			// Only update the time since start if the ParticleSystem is in the 'start' state.
			// Stop the ParticleSystem if the fixed timeout has been reached (if applicable).
			mTimeElapsedSinceStart += timeElapsed;
			if (mFixedTimeoutSet)
			{
				if (mTimeElapsedSinceStart >= mFixedTimeout)
				{
					// Stop the ParticleSystem
					stop();
					return;
				}
			}

			// Update bound timer (if not auto updated)
			if (!mBoundsAutoUpdate && mBoundsUpdateTime > 0.0f)
				mBoundsUpdateTime -= timeElapsed;

			// Calculate rotation of the node
			calulateRotationOffset();

			// Determine whether timeElapsed or iterationInterval is used
			size_t particlesLeft;
			if (mIterationIntervalSet)
			{
				// Update time since last update
				mTimeSinceLastUpdate += timeElapsed;
				while (mTimeSinceLastUpdate >= mIterationInterval)
				{
					// Update all techniques using the iteration interval value
					particlesLeft = _updateTechniques(mIterationInterval);
					mTimeSinceLastUpdate -= mIterationInterval;
				}
			}
			else
			{
				// Update all techniques using the time elapsed (since last frame)
				particlesLeft = _updateTechniques(timeElapsed);
			}

			// Determine whether the particle system should be stopped because of a fade out
			if (mStopFadeSet && particlesLeft == 0)
			{
				// Stop the ParticleSystem
				stop();
				return;
			}
		}
		else if (mState == ParticleSystem::PSS_PREPARED)
		{
			// Prepare all techniques (perform some initialisation in advance)
			ParticleTechniqueIterator it;
			for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
			{
				(*it)->_prepare();
			}
			
			// Only do it once.
			mState = ParticleSystem::PSS_STOPPED;
		}
		else if (mState == ParticleSystem::PSS_PAUSED)
		{
			// Determine whether there is a limit to the pause
			if (mPauseTimeSet)
			{
				mPauseTimeElapsed += timeElapsed;
				if (mPauseTimeElapsed > mPauseTime)
				{
					mPauseTimeElapsed = 0.0f;
					resume();
				}
			}
		}

		// Set the latest position. This value is always needed, whether the Particle System is emitted or not.
		latestPosition = getDerivedPosition();
		mLatestOrientation = getDerivedOrientation();
	}
	//-----------------------------------------------------------------------
	size_t ParticleSystem::_updateTechniques(Ogre::Real timeElapsed)
	{
		/** Update all techniques if particle system is started (only if the techniques aren't emitted 
			themselves) and return the total number of emitted particles.
			Note, that emitted techniques are updated by the technique that is responsible for emitting 
			them. The technique that is emitting pooled techniques is also responsible to take the 
			(world)AABB of that pooled technique into account.
		*/
		ParticleTechniqueIterator it;
		size_t particlesLeft = 0;
		bool mAABBUpdate = mParentNode && (mBoundsAutoUpdate || mBoundsUpdateTime > 0.0f);
		bool merge = mAABBUpdate;
		Ogre::AxisAlignedBox worldAABB(mParentNode->_getDerivedPosition(), mParentNode->_getDerivedPosition());
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			if (!(*it)->_isMarkedForEmission())
			{
				// Only call this if update bounds is needed.
				if (merge)
				{
					// Call _notifyUpdateBounds() for each Particle Technique, so the mWorldAABB in the 
					// ParticleTechnique::_update() function is (re)calculated.
					(*it)->_notifyUpdateBounds();
				}

				// Always update the ParticleTechniques
				(*it)->_update(timeElapsed);

				// Merge worldAABB's of all ParticleTechniques
				if (merge)
				{
					// Get the WorldAABB from each technique and merge it with the worldAABB.
					worldAABB.merge((*it)->getWorldBoundingBox());

					// If worldAABB is infinite, ignore the other Particle Techniques.
					if (worldAABB.isInfinite())
					{
						merge = false;
					}
				}

				// Count all left particles, used for anyone who needs it.
				particlesLeft += (*it)->getNumberOfEmittedParticles();
			}
		}

		if (mState == ParticleSystem::PSS_STOPPED)
		{
			/** Don't bother to update the mAABB, because the system is stopped in one of the techniques.
				The bounds must be reset, because even though the stop() has been called (which alread resets the
				bounds) another technique in the list might set the bounds again.
			*/
			_resetBounds();
		}
		else if (mAABBUpdate)
		{
			// If needed, update mAABB
			if (!worldAABB.isNull())
			{
				if (mTightBoundingBox)
				{
					// Wrap the bounding box tight around the particle system
					mAABB = worldAABB;
					mAABB.transformAffine(mParentNode->_getFullTransform().inverseAffine());
				}
				else
				{
					// Merge with the current bounding box
					// Note, that the mAABB must in localspace, so transformation of the worldAABB is required.
					Ogre::AxisAlignedBox newAABB(worldAABB);
					newAABB.transformAffine(mParentNode->_getFullTransform().inverseAffine());
	
					// Merge calculated box with current AABB.
					mAABB.merge(newAABB);
				}

				// Update bounding radius
				Ogre::Real sqDist = std::max(mAABB.getMinimum().squaredLength(), mAABB.getMaximum().squaredLength());
				mBoundingRadius = Ogre::Math::Sqrt(sqDist);
			}
			else
			{
				_resetBounds();
			}

			// Notify the parent node that the bounds are changed
			mParentNode->needUpdate();
		}

		return particlesLeft;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_resetBounds(void)
	{
		// Reset the bounds to zero
		if (!mAABB.isNull())
		{
			mAABB.setNull();
		}
		mBoundingRadius = 0.0;
		if (mParentNode)
		{
			mParentNode->needUpdate();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setBoundsAutoUpdated(bool autoUpdate, Ogre::Real stopIn)
	{
		mBoundsAutoUpdate = autoUpdate;
		mBoundsUpdateTime = stopIn;
		mOriginalBoundsUpdateTime = stopIn;
	}
	//-----------------------------------------------------------------------
	const Ogre::Vector3& ParticleSystem::getScale(void) const
	{
		return mParticleSystemScale;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setScale(const Ogre::Vector3& scale)
	{
		// Set the scale and notify the particle techniques
		mParticleSystemScale = scale;
		_notifyRescaled();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_notifyRescaled(void)
	{
		// Notify the particle techniques
		ParticleTechniqueConstIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_notifyRescaled(mParticleSystemScale);
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Real& ParticleSystem::getScaleVelocity(void) const
	{
		return mParticleSystemScaleVelocity;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setScaleVelocity(const Ogre::Real& scaleVelocity)
	{
		// Set the scale and notify the particle techniques
		mParticleSystemScaleVelocity = scaleVelocity;
		_notifyVelocityRescaled();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_notifyVelocityRescaled(void)
	{
		// Notify the particle techniques
		ParticleTechniqueConstIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_notifyVelocityRescaled(mParticleSystemScaleVelocity);
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Real& ParticleSystem::getScaleTime(void) const
	{
		return mParticleSystemScaleTime;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setScaleTime(const Ogre::Real& scaleTime)
	{
		mParticleSystemScaleTime = scaleTime;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_initForEmission(void)
	{
		// The system itself is emitted.
		Particle::_initForEmission();

		// Inherited from the Particle class and is only called if the Particle System is emitted.
		start();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_initForExpiration(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		// The system itself is expired.
		Particle::_initForExpiration(technique, timeElapsed);

		// Inherited from the Particle class and is only called if the Particle System is emitted.
		stop();
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_process(ParticleTechnique* technique, Ogre::Real timeElapsed)
	{
		// Inherited from the Particle class and is only called if the Particle System is emitted.
		// Update node position.
		Particle::_process(technique, timeElapsed);
		Ogre::Node* node = technique->getParentSystem()->getParentNode();
		if (mParentNode && node)
		{
			// position attribute is derived, but the the parentNode position must be set in relation to its parent.
			mParentNode->setPosition(position - node->getPosition());
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Real ParticleSystem::getNonVisibleUpdateTimeout(void) const
	{
		return mNonvisibleUpdateTimeout;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setNonVisibleUpdateTimeout(Ogre::Real timeout)
	{
		if (timeout > 0)
		{
			mNonvisibleUpdateTimeout = timeout;
			mNonvisibleUpdateTimeoutSet = true;
		}
		else
		{
			mNonvisibleUpdateTimeout = 0;
			mNonvisibleUpdateTimeoutSet = false;
		}
	}
	// --------------------------------------------------------------------
	const ParticleSystem::LodDistanceList& ParticleSystem::getLodDistances(void) const
	{
		return mLodDistanceList;
	}
	// --------------------------------------------------------------------
	void ParticleSystem::clearLodDistances(void)
	{
		mLodDistanceList.clear();
	}
	// --------------------------------------------------------------------
	void ParticleSystem::addLodDistance(Ogre::Real distance)
	{
		mLodDistanceList.push_back(distance * distance);
	}
	// --------------------------------------------------------------------
	void ParticleSystem::setLodDistances(const LodDistanceList& lodDistances)
	{
		LodDistanceConstIterator it, itEnd;
		itEnd = lodDistances.end();
		mLodDistanceList.clear();
		for (it = lodDistances.begin(); it != itEnd; ++it)
		{
			mLodDistanceList.push_back((*it) * (*it));
		}
	}
	// --------------------------------------------------------------------
	unsigned short ParticleSystem::_getLodIndex(Ogre::Real distance) const
	{
		return _getLodIndexSquaredDistance(distance * distance);
	}
	// --------------------------------------------------------------------
	unsigned short ParticleSystem::_getLodIndexSquaredDistance(Ogre::Real squaredDistance) const
	{
		LodDistanceConstIterator it, itEnd;
		itEnd = mLodDistanceList.end();
		unsigned short index = 0;
		for (it = mLodDistanceList.begin(); it != itEnd; ++it, ++index)
		{
			if (*it > squaredDistance)
			{
				return index - 1;
			}
		}

		// No match, so use max. value
		return static_cast<unsigned short>(mLodDistanceList.size() - 1);
	}
	//-----------------------------------------------------------------------
	size_t ParticleSystem::getNumEmittedTechniques (void) const
	{
		ParticleTechniqueConstIterator it;
		size_t count = 0;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			if ((*it)->_isMarkedForEmission())
			{
				count++;
			}
		}

		return count;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_markForEmission(void)
	{
		// Reset the marks on all techniques.
		_resetMarkForEmission();

		/** Force techniques to be enabled to mark the emitted objects. Do this first for all techniques, before
			the techniques perform their marking. The reason for this is, that one technique might mark 
			another technique.
		*/
		ParticleTechniqueConstIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->suppressNotifyEmissionChange(false);
		}

		// Mark the emitted objects.
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_markForEmission();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_resetMarkForEmission(void)
	{
		ParticleTechniqueConstIterator it;
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_setMarkedForEmission(false);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::suppressNotifyEmissionChange(bool suppress)
	{
		mSuppressNotifyEmissionChange = suppress;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::_notifyEmissionChange(void)
	{
		if (mSuppressNotifyEmissionChange)
			return;

		// Mark all emitted techniques, which on their turn mark other techniques, emitters and affectors.
		_markForEmission();
	}
	//-----------------------------------------------------------------------
	const Ogre::Real ParticleSystem::getIterationInterval(void) const
	{
		return mIterationInterval;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setIterationInterval(const Ogre::Real iterationInterval)
	{
		if (iterationInterval > 0)
		{
			mIterationInterval = iterationInterval;
			mIterationIntervalSet = true;
		}
		else
		{
			mIterationInterval = 0;
			mIterationIntervalSet = false;
		}
	}
	//-----------------------------------------------------------------------
	const Ogre::Real ParticleSystem::getFixedTimeout(void) const
	{
		return mFixedTimeout;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::setFixedTimeout(const Ogre::Real fixedTimeout)
	{
		if (fixedTimeout > 0.0f)
		{
			mFixedTimeout = fixedTimeout;
			mFixedTimeoutSet = true;
		}
		else
		{
			mFixedTimeout = 0.0f;
			mFixedTimeoutSet = false;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::prepare (void)
	{
		mState = ParticleSystem::PSS_PREPARED;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::start (void)
	{
		setVisible(true);
		mState = ParticleSystem::PSS_STARTED;
		mTimeElapsedSinceStart = 0;
		mLastVisibleFrame = 0;
		mTimeSinceLastVisible = 0;
		mTimeSinceLastUpdate = 0;
		mBoundsUpdateTime = mOriginalBoundsUpdateTime;
		ParticleTechniqueIterator it;
		mAABB.setExtents(0, 0, 0, 0, 0, 0);
		mFastForwardSet = mOriginalFastForwardSet;
		position = Ogre::Vector3::ZERO;
		mDerivedPosition = Ogre::Vector3::ZERO;
		latestPosition = getDerivedPosition();
		mEnabled = true;
		mStopFadeSet = false;

		// Run through the ParticleTechniques to perform some start-initialisation actions.
		for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
		{
			(*it)->_notifyStart();
		}
		
		// Perform fast forward if needed (the fastforward() function determines whether it is needed)
		fastForward();

		// Run through all listeners
		if (mParticleSystemListenerList.empty())
			return;

		ParticleSystemListenerIterator itpsl;
		ParticleSystemListenerIterator itpslEnd = mParticleSystemListenerList.end();
		for (itpsl = mParticleSystemListenerList.begin(); itpsl != itpslEnd; ++itpsl)
		{
			(*itpsl)->started();
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::stop (void)
	{
		/*  Note, that the ParticleSystem ins´t visible anymore, but it is still attached to the
			node and still consumes resources, but it allows fast start/stop iterations if needed.
			An important thing why it keeps attached to the node is that attaching and detaching the 
			ParticleSystem to/from a node must be done outside of the ParticleSystem. If the ParticleSystem
			isn't used anymore you have to detach it from the node yourself.
		*/
		if (mState != ParticleSystem::PSS_STOPPED)
		{
			/** Notify all techniques to stop.
			*/
			setVisible(false); // Set this movable to invisible
			ParticleTechniqueIterator it;
			for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
			{
				(*it)->_notifyStop();
			}

			_resetBounds();
			mState = ParticleSystem::PSS_STOPPED;

			// Run through all listeners
			if (mParticleSystemListenerList.empty())
				return;

			ParticleSystemListenerIterator itpsl;
			ParticleSystemListenerIterator itpslEnd = mParticleSystemListenerList.end();
			for (itpsl = mParticleSystemListenerList.begin(); itpsl != itpslEnd; ++itpsl)
			{
				(*itpsl)->stopped();
			}
			mStopFadeSet = false;
			mEnabled = false;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::stop (Ogre::Real stopTime)
	{
		setFixedTimeout(stopTime);
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::stopFade (void)
	{
		size_t i;
		size_t j;
		size_t numTechniques = getNumTechniques();
		size_t numEmitters;
		ParticleTechnique* technique;
		ParticleEmitter* emitter;
		for (i = 0; i < numTechniques; ++i)
		{
			technique = getTechnique(i);
			numEmitters = getTechnique(i)->getNumEmitters();
			for (j = 0; j < numEmitters; ++j)
			{
				emitter = technique->getEmitter(j);
				emitter->setEnabled(false);
			}
		}
		mStopFadeSet = true;
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::pause (void)
	{
		if (mState != ParticleSystem::PSS_STOPPED)
		{
			mState = ParticleSystem::PSS_PAUSED;

			ParticleTechniqueIterator it;
			for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
			{
				(*it)->_notifyPause();
			}
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::pause (Ogre::Real pauseTime)
	{
		pause();
		setPauseTime(pauseTime);
	}
	//-----------------------------------------------------------------------
	void ParticleSystem::resume (void)
	{
		if (mState != ParticleSystem::PSS_STOPPED)
		{
			mState = ParticleSystem::PSS_STARTED;

			ParticleTechniqueIterator it;
			for (it = mTechniques.begin(); it != mTechniques.end(); ++it)
			{
				(*it)->_notifyResume();
			}
		}
	}
}

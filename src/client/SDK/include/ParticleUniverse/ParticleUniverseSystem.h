/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SYSTEM_H__
#define __PU_SYSTEM_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseParticle.h"
#include "ParticleUniverseTechnique.h"
#include "ParticleUniverseIElement.h"
#include "ParticleUniverseSystemListener.h"

namespace ParticleUniverse
{
	/** A ParticleSystem is the most top level of a particle structure, that consists of Particles, ParticleEmitters, 
		ParticleAffectors, ParticleObservers, etc. 
	@remarks
		The ParticleSystem can be seen as the container that includes the components that are needed to create, 
		display and move particles.
    */
	class _ParticleUniverseExport ParticleSystem : public Ogre::MovableObject, public Particle, public IElement
	{
		public:
			// Default values
			static const bool DEFAULT_KEEP_LOCAL;
			static const Ogre::Real DEFAULT_ITERATION_INTERVAL;
			static const Ogre::Real DEFAULT_FIXED_TIMEOUT;
			static const Ogre::Real DEFAULT_NON_VISIBLE_UPDATE_TIMEOUT;
			static const bool DEFAULT_SMOOTH_LOD;
			static const Ogre::Real DEFAULT_FAST_FORWARD_TIME;
			static const Ogre::String DEFAULT_MAIN_CAMERA_NAME;
			static const Ogre::Real DEFAULT_SCALE_VELOCITY;
			static const Ogre::Real DEFAULT_SCALE_TIME;
			static const Ogre::Vector3 DEFAULT_SCALE;
			static const bool DEFAULT_TIGHT_BOUNDINGBOX;
			
			enum ParticleSystemState
			{
				PSS_PREPARED,
				PSS_STARTED,
				PSS_STOPPED,
				PSS_PAUSED
			};

			enum ComponentType
			{
				CT_VISUAL_PARTICLE,
				CT_TECHNIQUE,
				CT_EMITTER,
				CT_AFFECTOR,
				CT_OBSERVER
			};

			/** Distance list used to specify LOD
		    */
			typedef std::vector<Ogre::Real> LodDistanceList;
			typedef LodDistanceList::iterator LodDistanceIterator;
			typedef LodDistanceList::const_iterator LodDistanceConstIterator;

			typedef std::vector<ParticleTechnique*> ParticleTechniqueList;
			typedef ParticleTechniqueList::iterator ParticleTechniqueIterator;
			typedef ParticleTechniqueList::const_iterator ParticleTechniqueConstIterator;

			typedef std::vector<ParticleSystemListener*>::iterator ParticleSystemListenerIterator;
			typedef std::vector<ParticleSystemListener*> ParticleSystemListenerList;

			ParticleSystem(const Ogre::String& name);
			ParticleSystem(const Ogre::String& name, const Ogre::String& resourceGroupName);
	        virtual ~ParticleSystem(void);

			/** Gets the derived position of the particle system (i.e derived from the parent node, if available).
	        */
			inline virtual const Ogre::Vector3& getDerivedPosition(void);

			/** Gets the derived orientation of the particle system (i.e derived from the parent node, if available).
	        */
			inline virtual const Ogre::Quaternion& getDerivedOrientation(void) const;

			/** Gets the latest orientation of the particle system before update.
	        */
			inline const Ogre::Quaternion& getLatestOrientation(void) const;

			/** Returns true if the particle system has rotated between updates.
	        */
			inline bool hasRotatedBetweenUpdates(void) const;

			/** If the orientation of the particle system has been changed since the last update, the passed vector
				is rotated accordingly.
	        */
			inline void rotationOffset(Ogre::Vector3& pos);

			/** Is smooth Lod set?.
	        */
			inline bool isSmoothLod (void) const {return mSmoothLod;};

			/** Set smooth Lod.
	        */
			void setSmoothLod (bool smoothLod) {mSmoothLod = smoothLod;};

			/** Returns the time since the ParticleSystem was started.
	        */
			inline Ogre::Real getTimeElapsedSinceStart(void) const {return mTimeElapsedSinceStart;};

			/** Assignment operator.
			*/
			ParticleSystem& operator=(const ParticleSystem& ps);
		
			/** Get the name of the resource group that is assigned to this ParticleSystem.
			*/
			const Ogre::String& getResourceGroupName(void) const {return mResourceGroupName;};

			/** Set the name of the resource group for this ParticleSystem.
			*/
			void setResourceGroupName(const Ogre::String& resourceGroupName){mResourceGroupName = resourceGroupName;};

			/** Create a ParticleTechnique and add it to this ParticleSystem.
	        */
			ParticleTechnique* createTechnique(void);

			/** Add a technique to the list.
			@remarks
				It must be possible to add a previously created technique to the list. This is the case with 
				techniques that were created outside the particle system. An example is the creation of 
				techniques by means of a script. The technique will be placed under control of the particle 
				system.	The Particle System Manager however, deletes the techniques (since they are also 
				created by the Particle System Manager).
			@param
				technique Pointer to a previously created technique.
	        */
			void addTechnique (ParticleTechnique* technique);

			/** Remove a technique from the system, but don't destroy it.
	        */
			void removeTechnique (ParticleTechnique* technique);

			/** Get a ParticleTechnique given a certain index.
	        */
			ParticleTechnique* getTechnique (size_t index) const;

			/** Get a ParticleTechnique. Search by ParticleTechnique name.
	        */
			ParticleTechnique* getTechnique (const Ogre::String& name) const;

			/** Get the number of ParticleTechniques of this ParticleSystem.
	        */
			size_t getNumTechniques (void) const;

			/** Destroy a ParticleTechnique.
	        */
			void destroyTechnique(ParticleTechnique* particleTechnique);

			/** Destroy a ParticleTechnique identified by means of an index.
	        */
			void destroyTechnique (size_t index);

			/** Destroy all ParticleTechniques of this ParticleSystem.
	        */
			void destroyAllTechniques (void);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			inline virtual void _notifyCurrentCamera(Ogre::Camera* cam);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			const Ogre::String& getMovableType(void) const;

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			const Ogre::AxisAlignedBox& getBoundingBox(void) const;

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			virtual Ogre::Real getBoundingRadius(void) const;
			
			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			inline virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

			/** Overridden from MovableObject
			@see
				MovableObject
			*/
			virtual void setRenderQueueGroup(Ogre::uint8 queueId);

			/** Updates the particle system based on time elapsed.
			@remarks
				This is called automatically every frame by OGRE.
			@param
				timeElapsed The amount of time, in seconds, since the last frame.
			*/
			void _update(Ogre::Real timeElapsed);

			/** Update all techniques and return the number of emitted particles for any function who is interested.
			@param
				timeElapsed The amount of time, in seconds, since the last frame.
			*/
			size_t _updateTechniques(Ogre::Real timeElapsed);

			/** 
			*/
			const Ogre::Real getNonVisibleUpdateTimeout(void) const;

			/** 
			*/
			void setNonVisibleUpdateTimeout(Ogre::Real timeout);

			/** Prepares the particle system.
			@remarks
				This optional state can be used to prepare the particle system, so that initial actions are
				preformed before the particle system is started.
			*/
			void prepare (void);

			/** Starts the particle system.
			@remarks
				Only if a particle system has been attached to a SceneNode it can be started.
			*/
			void start (void);

			/** Stops the particle system.
			@remarks
				Only if a particle system has been attached to a SceneNode and started it can be stopped.
			*/
			void stop (void);

			/** Stops the particle system after a period of time.
			@remarks
				This is basicly the same as calling setFixedTimeout().
			*/
			void stop (Ogre::Real stopTime);

			/** Stops emission of all particle and really stops the particle system when all particles are expired.

			*/
			void stopFade (void);

			/** Pauses the particle system.
			*/
			void pause (void);

			/** Pauses the particle system for a period of time. After this time, the Particle System automatically resumes.
			*/
			void pause (Ogre::Real pauseTime);

			/** Resumes the particle system.
			*/
			void resume (void);

			/** Returns the state of the particle system.
			*/
			ParticleSystemState getState (void) const {return mState;};

			/** 
			*/
			const Ogre::Real getFastForwardTime(void) const;

			/** 
			*/
			const Ogre::Real getFastForwardInterval(void) const;

			/** 
			*/
			void setFastForward(Ogre::Real time, Ogre::Real interval);

			/** Get the main camera (name).
			*/
			const Ogre::String& getMainCameraName(void) const;
			Ogre::Camera* getMainCamera(void);

			/** True if the main camera has been set.
			*/
			inline bool hasMainCamera(void);

			/** Set the main camera (name).
			@remarks
				By setting the main camera, the number of cameras for which un update is performed,
				is narrowed. Particle System LOD for example only really works with a 1-camera setup, 
				so setting a camera (name) basicly prevents that LODding isn't screwed up if the scene 
				contains multiple cameras.
			*/
			void setMainCameraName(Ogre::String cameraName);
			void setMainCamera(Ogre::Camera* camera);

			/** Returns a pointer to the current camera (the one that gets updated/notified).
			*/
			Ogre::Camera* getCurrentCamera(void);

			/** 
			*/
			void fastForward(void);

			/** Determines whether the parent is a TagPoint or a SceneNode.
			*/
			inline bool isParentIsTagPoint(void) {return mParentIsTagPoint;};

			/** Returns the distances at which level-of-detail (LOD) levels come into effect.
			*/
			const LodDistanceList& getLodDistances(void) const;

			/** Clears the list with distances.
			*/
			void clearLodDistances(void);

			/** Adds a distance at which level-of-detail (LOD) levels come into effect.
			*/
			void addLodDistance(Ogre::Real distance);

			/** Sets the distance at which level-of-detail (LOD) levels come into effect.
			@remarks
				You should only use this if you have assigned LOD indexes to the ParticleTechnique
				instances attached to this ParticleSystem.
			@param
				lodDistances A vector of Reals which indicate the distance at which to switch to 
				another ParticleTechnique.
			*/
			void setLodDistances(const LodDistanceList& lodDistances);

			/** Returns the number of emitted ParticleTechniques.
			@remarks
				Emitted ParticleTechniques are determined by means of the _markForEmission() function.
	        */
			size_t getNumEmittedTechniques (void) const;

			/** Mark all emitted objects (ParticleTechniques and all the objects registered 
				at each ParticleTechnique).
	        */
			void _markForEmission(void);

			/** Reset mark for emission indication for all techniques.
	        */
			void _resetMarkForEmission(void);

			/** Function to suppress notification of an emission change.
			@remarks
				This function is typically used when notification isn´t needed anymore. An example for this 
				situation is for instance in case the ParticleTechnique is destroyed, which also causes the
				emitters to be destroyed.
	        */
			void suppressNotifyEmissionChange(bool suppress);

			/** Is called as soon as a new technique is added or deleted, which leads to a re-evaluation of the 
				emitted objects.
			*/
			void _notifyEmissionChange(void);

			/** 
	        */
			inline const Ogre::Real getIterationInterval(void) const;
			void setIterationInterval(const Ogre::Real iterationInterval);

			/** 
	        */
			inline const Ogre::Real getFixedTimeout(void) const;
			void setFixedTimeout(const Ogre::Real fixedTimeout);

			/** Sets whether the bounds will be automatically updated for the life of the particle system.
			*/
			void setBoundsAutoUpdated(bool autoUpdate, Ogre::Real stopIn = 0.0f);

			/** Reset the bounding box of the particle system to the most minimal value.
			*/
			void _resetBounds(void);

			/** Returns the scale.
			*/
			inline const Ogre::Vector3& getScale(void) const;

			/** Set the scale independant from the node to which it is attached.
			*/
			void setScale(const Ogre::Vector3& scale);

			/** Notify registered ParticleTechniques.
			*/
			void _notifyRescaled(void);
			
			/** Returns the velocity scale.
			*/
			const Ogre::Real& getScaleVelocity(void) const;

			/** Set the velocity scale.
			*/
			void setScaleVelocity(const Ogre::Real& scaleVelocity);

			/** Notify registered ParticleTechniques.
			*/
			void _notifyVelocityRescaled(void);

			/** Get the time scale.
			*/
			inline const Ogre::Real& getScaleTime(void) const;

			/** Set the time scale.
			*/
			void setScaleTime(const Ogre::Real& scaleTime);

			/** @copydoc Particle::_initForEmission */
			virtual void _initForEmission(void);

			/** @copydoc Particle::_initForExpiration */
			virtual void _initForExpiration(ParticleTechnique* technique, Ogre::Real timeElapsed);

			/** @copydoc Particle::_process */
			virtual void _process(ParticleTechnique* technique, Ogre::Real timeElapsed);

			/** 
			*/
			inline bool isKeepLocal(void) const;

			/** If this attribute is set to 'true', the particles are emitted relative to the system 
			*/
			void setKeepLocal(bool keepLocal);

			/** Transforms the particle position in a local position relative to the system
			*/
			inline bool makeParticleLocal(Particle* particle);

			/** Is 'true' when the bounding box is wrapped tight around the particle system or 'false' when the
				bounding box is only increasing and doesn´t shrink when the particle system shrinks.
			*/
			inline bool hasTightBoundingBox(void) const;

			/** Set mTightBoundingBox.
			*/
			void setTightBoundingBox(bool tightBoundingBox);

			/** Add a ParticleSystemListener, which gets called as soon as a ParticleSystem is started or stopped.
			*/
			void addParticleSystemListener (ParticleSystemListener* particleSystemListener);

			/** Removes the ParticleSystemListener, but it the ParticleSystemListener isn't destroyed.
			*/
			void removeParticleSystemListener (ParticleSystemListener* particleSystemListener);

			/** @see MovableObject
		    */
			virtual void visitRenderables (Ogre::Renderable::Visitor* visitor,
				bool debugRenderables = false) {/* No implementation */};

			/** Returns the time of a pause (if set)
			*/
			Ogre::Real getPauseTime (void) const;

			/** Set the pause time. This is used to pause the ParticleSystem for a certain amount of time.
			*/
			void setPauseTime (Ogre::Real pauseTime);

			/** Returns the name of the template which acted as a blueprint for creation of this Particle System.
			*/
			const Ogre::String& getTemplateName(void) const;

			/** Set the name of the template which acts as a blueprint for creation of this Particle System.
			*/
			void setTemplateName(const Ogre::String& templateName);

			/** See Particle::setEnabled
	        */
			virtual void setEnabled(bool enabled);

			/** Get the SceneManager with which the ParticleSystem is created
	        */
			inline Ogre::SceneManager* getSceneManager(void);

			/** Set the SceneManager with which the ParticleSystem is created
	        */
			void setSceneManager(Ogre::SceneManager* sceneManager);

		protected:
			/** Gets the LOD index to use at the given distance.
			*/
			unsigned short _getLodIndex(Ogre::Real distance) const;

            /** Gets the LOD index to use at the given squared distance.
			*/
			unsigned short _getLodIndexSquaredDistance(Ogre::Real squaredDistance) const;

            /** Calculate rotation of the node.
			*/
			inline void calulateRotationOffset(void);

			/** State of the particle system.
			@remarks
				Possible states are started, stopped, paused and resumed.
			*/
			ParticleSystemState mState;

			Ogre::AxisAlignedBox mAABB;
			Ogre::Real mBoundingRadius;

			/*	ParticleTechniques that are part of this ParticleSystem.
			*/
			ParticleTechniqueList mTechniques;

			/** Controller for time update
			*/
			Ogre::Controller<Ogre::Real>* mTimeController;

			/** Amount of time non-visible so far
			*/
			Ogre::Real mTimeSinceLastVisible;

			/** Last frame in which known to be visible
			*/
			unsigned long mLastVisibleFrame;

			/** Update timeout when nonvisible (0 for no timeout)
			*/
			Ogre::Real mNonvisibleUpdateTimeout;

			/** Update timeout when nonvisible set?
			*/
			bool mNonvisibleUpdateTimeoutSet;

			/** Counts the time since the ParticleSystem was started.
	        */
			Ogre::Real mTimeElapsedSinceStart;

			/** Name of the resource group that is assigned to this ParticleSystem.
	        */
			Ogre::String mResourceGroupName;

			/** List with LOD (= Level Of Detail) distances.
			@remarks
				The distance between a ParticleTechnique and the camera corresponds with a certain index. If the 
				value of that index correponds with the index set in the ParticleTechnique, the ParticleTechnique
				is enabled.
			@par
				If no ParticleTechnique with a corresponding index can be found, nothing is shown. An example:
				mLodDistanceList contains the values 300, 600 and 1200. If 2 techniques are defined with index 0
				(distance 0-300) and index 1 (300-600), all distances beyond 600 doesn't display a ParticleTechnique.
	        */
			LodDistanceList mLodDistanceList;

			/** Determines whether Lodding is performed instantly of smoothly.
			@remarks
				Setting this value to 'true' causes techniques (that are disabled because they come outside their Lod
				distance) to handle already emitted particles. New particles aren't emitted anymore, but the already
				emitted particles are still processed. This prevents an abrupt transition between one
				technique and another. Note, that the tradeoff for using Smooth Lod is, that it only works correct
				with 1 camera.
	        */
			bool mSmoothLod;

			/** Indication whether _notifyEmissionChange() must be suppressed or not.
			@remarks
				Default is true
	        */
			bool mSuppressNotifyEmissionChange;

			/** Value that determines in which intervals the system must be the updated.
	        */
			Ogre::Real mIterationInterval;

			/** Indication to determine whether updating of the technique must be done in predefined intervals.
	        */
			bool mIterationIntervalSet;

			/** Attribute used to calculate the time since the last update.
	        */
			Ogre::Real mTimeSinceLastUpdate;

			/** If set, the ParticleSystem automatically stops after ´mFixedTimeout´ seconds.
	        */
			Ogre::Real mFixedTimeout;

			/** Indication to determine whether mFixedTimeout has been set.
	        */
			bool mFixedTimeoutSet;

			/** Determines whether the recalculation of the bounding box is performed automatically.
	        */
			bool mBoundsAutoUpdate;

			/** Time that indicates how long the AABB must be calculated before it is fixed.
	        */
			Ogre::Real mBoundsUpdateTime;

			/** Original value of mBoundsUpdateTime.
	        */
			Ogre::Real mOriginalBoundsUpdateTime;

			/** Fastforward settings
	        */
			bool mFastForwardSet;
			bool mOriginalFastForwardSet;
			Ogre::Real mFastForwardTime;
			Ogre::Real mFastForwardInterval;

			/** If the mainCameraName has been set, the number of updates for other cameras is limited.
	        */
			Ogre::String mMainCameraName;
			bool mMainCameraNameSet;
			Ogre::Camera* mCurrentCamera;

			/** Scale the Particle Systems´ relative positions and size.
			@remarks
				Scaling is done on a Particle System level and is independent of the scaling of the SceneNode
		    */
			Ogre::Vector3 mParticleSystemScale;

			/** Scale velocity.
			@remarks
				Scaling of the particle velocity is independant from scaling the positions and size.
		    */
			Ogre::Real mParticleSystemScaleVelocity;

			/** Scale time.
			@remarks
				Makes the Particle System speed up or slow down.
		    */
			Ogre::Real mParticleSystemScaleTime;

			/** Determines whether particle positions should be kept local in relation to the system.
	        */
			bool mKeepLocal;

			/** Determines whether the bounding box is tight around the particle system or whether the bounding
			    is growing and never shrinks.
	        */
			bool mTightBoundingBox;

			/** List of ParticleSystemListeners
	        */
			ParticleSystemListenerList mParticleSystemListenerList;

			/** The Particle System can be paused for a specific time
	        */
			Ogre::Real mPauseTime;
			bool mPauseTimeSet;
			Ogre::Real mPauseTimeElapsed;

			/** The name of the template on which this Particle System is derived.
	        */
			Ogre::String mTemplateName;

			/** The stopFade() function is called if set to true.
	        */
			bool mStopFadeSet;

			/** The SceneManager with which the Particle System is created.
			*/
			Ogre::SceneManager* mSceneManager;

			/** To keep track of rotation between updates, the latest rotation must be kept.
			*/
			Ogre::Quaternion mLatestOrientation;

			/** Rotation offset between 2 updates.
			*/
			Ogre::Quaternion mRotationOffset;

			/** The rotation centre.
			*/
			Ogre::Vector3 mRotationCentre;
	};

}
#endif

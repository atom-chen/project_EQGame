/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2009 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PHYSX_EXTERN_TOKENS_H__
#define __PU_PHYSX_EXTERN_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#ifdef PU_PHYSICS_PHYSX
#include "ParticleUniverseExternTokens.h"

namespace ParticleUniverse
{
	/** This class contains the tokens that are needed for the PhysXExtern.
    */
	class _ParticleUniverseExport PhysXExternTokens : public ExternTokens
	{
		public:
			static const Ogre::String KEYWORD_PHYSX;
			static const Ogre::String KEYWORD_SHAPE_TYPE;
			static const Ogre::String KEYWORD_ACTOR_COLLISION_GROUP;
			static const Ogre::String KEYWORD_SHAPE_COLLISION_GROUP;
			static const Ogre::String KEYWORD_GROUP_MASK;
			static const Ogre::String KEYWORD_ANGULAR_VELOCITY;
			static const Ogre::String KEYWORD_ANGULAR_DAMPING;
			static const Ogre::String KEYWORD_MATERIAL_INDEX;
			static const Ogre::String KEYWORD_BOX;
			static const Ogre::String KEYWORD_SPHERE;
			static const Ogre::String KEYWORD_CAPSULE;

			PhysXExternTokens(void) {};
			virtual ~PhysXExternTokens(void) {};

			/** @see
				ITokenBind::setupTokenDefinitions
			*/
			virtual void setupTokenDefinitions(ITokenRegister* tokenRegister);

			/** @see
				ITokenBind::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);

		protected:
			/** Tokens which get called during parsing.
			*/
			class ShapeTypeToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mShapeTypeToken;
			class ActorCollisionGroupToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mActorCollisionGroupToken;
			class ShapeCollisionGroupToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mShapeCollisionGroupToken;
			class GroupMaskToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mGroupMaskToken;
			class AngularVelociyToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mAngularVelociyToken;
			class AngularDampingToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mAngularDampingToken;
			class MaterialIndexToken : public IToken {virtual void parse(ParticleScriptDeserializer* serializer);} mMaterialIndexToken;
	};

}
#endif //PU_PHYSICS_PHYSX
#endif
